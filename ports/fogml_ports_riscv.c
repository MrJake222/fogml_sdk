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

#if defined(__riscv)

#include "fogml_ports.h"

static unsigned long my_rand_state = 42;

int fogml_random(int min, int max)
{
    my_rand_state = (my_rand_state * 1103515245 + 12345) % 2147483648;
    return min + (my_rand_state % (max - min + 1));
}

extern int printf_(const char* format, ...);

void fogml_printf(char const *str)
{
    printf_("%s", str);
};

void fogml_printf_float(float number)
{
    printf_("%.2f", number);
};

void fogml_printf_int(int number)
{
    printf_("%d", number);
};

#endif
