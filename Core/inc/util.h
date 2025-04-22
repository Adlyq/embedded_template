//
// Created by 28331 on 25-2-12.
//

#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include <n32g031_tim.h>

#define __NORETURN __attribute__((noreturn))

#define LOOP_UNTIL(condition) while (!(condition)) { }
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef uint64_t u64;
typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

void SoftDelay(uint32_t nCount);

void TIM_InitOc(TIM_Module* TIMx, uint8_t oc,OCInitType* TIM_OCInitStruct);

#endif //UTIL_H
