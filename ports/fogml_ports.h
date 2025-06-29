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

#ifndef __FOGML_PORTS_H__
#define __FOGML_PORTS_H__

#ifdef ARDUINO
#include <Arduino.h>
#endif

#if defined(__ZEPHYR__)
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <zephyr.h>
#include <random/rand32.h>
#endif

#if defined(__riscv)
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int fogml_random(int min, int max);
void fogml_printf(char const *str);
void fogml_printf_float(float number);
void fogml_printf_int(int number);

#if defined(__riscv)
#include "fogml_ports_riscv_inline.h"
#else
	/* -------- custom square -------- */
	// should return x*x
	float fogml_pow2f(float x) { return x*x; }
	// profiled version, cm=multiply, cn=norm cycles
	float fogml_pow2fc(float f, fogml_xfpu_cycles *c) { return x*x; }

	/* -------- diff-square-accum -------- */
	// should perfrorm: acc += (x1-x2)^2
	void fogml_dsqa(float* acc, float x1, float x2) { *acc += (x1-x2)*(x1-x2); }
	// profiled version, cd=diff, csq=square, ca=accum cycles
	void fogml_dsqac(float* acc, float x1, float x2, fogml_xfpu_cycles *c) { *acc += (x1-x2)*(x1-x2); }
#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif
