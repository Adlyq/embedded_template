//
// Created by 28331 on 25-2-12.
//

#pragma once
#include "common_defs.h"
EXTERN_C_BEGIN

#include <stdint.h>
#include <n32g031_tim.h>
#include <stdlib.h>

#define LOOP_UNTIL(condition) while (!(condition)) { }
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define APPROX_EQUAL(a, b, EPSILON) (abs((int32_t)(a) - (int32_t)(b)) <= (EPSILON))

#if __has_attribute(optimize)
#define OPTIMIZE_O3 __attribute__((optimize("O3", "unroll-loops")))
#else
#define OPTIMIZE_O3
#endif

typedef uint64_t u64;
typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;

extern volatile u32 timestamp;

void delayMs(uint32_t ms);

void delayUs(u32 us);

u32 fast_rand(void);

void TIM_InitOc(TIM_Module* TIMx, uint8_t oc, OCInitType* TIM_OCInitStruct);


EXTERN_C_END
