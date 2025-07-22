//
// Created by 28331 on 25-2-12.
//

#pragma once

#include "common_defs.h"

EXTERN_C_BEGIN

#include <stdint.h>
#include <n32g031_tim.h>
#include <stdlib.h>

#define __NORETURN __attribute__((noreturn))

#define LOOP_UNTIL(condition) while (!(condition)) { }
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define APPROX_EQUAL(a, b, EPSILON) (abs((int32_t)(a) - (int32_t)(b)) <= (EPSILON))

typedef uint64_t u64;
typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;

extern volatile u32 timestamp;

void delayMs(uint32_t ms);

void delayUs(u32 us);

void TIM_InitOc(TIM_Module* TIMx, uint8_t oc, OCInitType* TIM_OCInitStruct);

EXTERN_C_END
