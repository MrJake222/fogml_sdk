#include "fogml_helper.h"

#include <stdint.h>
#include <math.h>

/*float pow2f(float a) {
	return a*a;
}*/

static int C() { int c; asm volatile ("rdcycle %0" : "=r"(c)); return c; }

uint32_t float_square_bitlevel(uint32_t x, int* cm, int* cr) {
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
    *cm += C()-s;

    // renormalize the fraction (renorm)
    s = C();
    while (fsqh || fsql & 0xFF000000) {
		// this loop is optimized into two separate loops
		fsql >>= 1;
		fsql |= fsqh << 31;		
        fsqh >>= 1;
        exp++;
    }
    *cr += C()-s;

    fsql &= ~0x00800000;
    fsql |= exp<<23;

    return fsql;
}

float pow2f(float f) {
    int cm, cr;
    return pow2fc(f, &cm, &cr);
}

float pow2fc(float f, int* cm, int* cr) {
	//float f_sq = f*f;
    uint32_t fu = *(uint32_t*)&f;
    uint32_t fu_sq = float_square_bitlevel(fu, cm, cr);
    float f_sq = *(float*)&fu_sq;
    /*if (fabsf(f_sq - f*f) > 0.001f) {
		fogml_printf("failed on: ");
		fogml_printf_float(f);
		fogml_printf("\n");
	}*/
    return f_sq;
}
