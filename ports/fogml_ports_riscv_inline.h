/*
   Copyright 2021 FogML
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
	   http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#define RISCV_XFPU
//#define RISCV_XFPU_CHECK

#ifndef RISCV_XFPU_HEADER
#define RISCV_XFPU_HEADER

#include "fogml_ports.h"

struct fogml_xfpu_cycles {
	int df;		// difference
	int sq;		// square
	int sqmul;	// mul 
	int sqnorm;	// norm
	int ac;		// accumulate
	
	int dist;	// distance function
	int rt;		// square root
};

/* ----------------------------- custom squaring ----------------------------- */
// defined in a header file for the compiler to properly
// inline these time-critical functions

static int C() { int c; asm volatile ("rdcycle %0" : "=r"(c)); return c; }
extern int printf_(const char* format, ...);

#ifndef RISCV_XFPU
static uint32_t float_square_bitlevel(uint32_t x, struct fogml_xfpu_cycles* c) {
	if (x == 0)
		return x;

	// extract the exponent, subtract bias
	// extract the fraction, add implicit 1
	uint32_t exp =  ((x>>23) & 0xFF) - 0x7F;
	uint64_t frac =  (x & 0x007FFFFF) | 0x00800000;

	exp *= 2;		// double the exponent
	exp += 0x68;	// add back the bias 0x7F minus 23

	// square the fraction (mul64)
	int s = C();
	frac *= frac;
	uint32_t fsql = (uint32_t)(frac);
	uint32_t fsqh = (uint32_t)(frac>>32);
	c->sqmul += C()-s;

	// normalize the fraction (norm)
	s = C();
	while (fsqh) {
		fsql >>= 1;
		fsql |= fsqh << 31;		
		fsqh >>= 1;
		exp++;
	}
	while (fsql & 0xFF000000) {
		fsql >>= 1;
		exp++;
	}
	c->sqnorm += C()-s;

	fsql &= ~0x00800000;
	fsql |= exp<<23;
	return fsql;
}
#endif

static float fogml_pow2fc(float f, struct fogml_xfpu_cycles* c) {
	float f_sq;
	
	#ifdef RISCV_XFPU
		// custom instruction
		asm("xfsq %[out], %[in]" : [out]"=r"(f_sq) : [in]"r"(f));
	#else
		// stdlib floating point
		//f_sq = f*f;
		// custom bitlevel with deeper profiling
		uint32_t fu = *(uint32_t*)&f;
		uint32_t fu_sq = float_square_bitlevel(fu, c);
		f_sq = *(float*)&fu_sq;
	#endif

	#ifdef RISCV_XFPU_CHECK
		// check the results
		if (fabsf(f_sq - f*f) > 0.001f) {
			fogml_printf("failed on: ");
			fogml_printf_float(f);
			fogml_printf("\n");
		}
	#endif
	
	return f_sq;
}

static float fogml_pow2f(float f) {
	struct fogml_xfpu_cycles c;
	return fogml_pow2fc(f, &c);
}

static void fogml_dsqac(float* acc, float x1, float x2, struct fogml_xfpu_cycles* c) {
	#ifdef RISCV_XFPU_CHECK
		float prevacc = *acc;
	#endif
	
	int s;
	#ifdef RISCV_XFPU
		// custom instruction
		s=C();  asm("dsqa %[out], %[in1], %[in2]" : [out]"+r"(*acc) : [in1]"r"(x1), [in2]"r"(x2));  c->sq+=C()-s;
	#else
		// stdlib floating point
		float x;
		s=C();  x = x1 - x2;             c->df+=C()-s;
		s=C();  x = fogml_pow2fc(x, c);  c->sq+=C()-s;
		s=C();  *acc += x;               c->ac+=C()-s;
	#endif

	#ifdef RISCV_XFPU_CHECK
		float expt = prevacc + (x1-x2)*(x1-x2);
		if (fabsf(*acc-expt)>1e-4f) {
			printf_("%12.4f %08lx + (%12.4f %08lx - %12.4f %08lx)^2 = %12.4f %08lx is %12.4f %08lx expt\n",
				(double)prevacc,	*(uint32_t*)&prevacc,
				(double)x1,			*(uint32_t*)&x1,
				(double)x2,			*(uint32_t*)&x2,
				(double)*acc,		*(uint32_t*)acc,
				(double)expt,		*(uint32_t*)&expt);
		}
	#endif
}

static void fogml_dsqa(float* acc, float x1, float x2) {
	struct fogml_xfpu_cycles c;
	fogml_dsqac(acc, x1, x2, &c);
}

#endif
