//
// Created by 28331 on 25-2-12.
//

#pragma once
#include "common_defs.h"
EXTERN_C_BEGIN

#include <stdint.h>
#include <n32g031_tim.h>

#define STR_HELPER(s) #s
#define STR(s) STR_HELPER(s)

#define LOOP_UNTIL(condition) while (!(condition)) { }
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define APPROX_EQUAL(a, b, EPSILON) (abs((int32_t)(a) - (int32_t)(b)) <= (EPSILON))

#if   defined ( __ICCARM__ )

#elif defined (__clang__)

#elif defined ( __GNUC__ )

#endif

#if __has_attribute(optimize)
#define OPTIMIZE_O3 __attribute__((optimize("O3", "unroll-loops")))
#else
#define OPTIMIZE_O3
#endif

#ifdef SEGGER_RTT
#include <stdio.h>

#define FUNCTION_NAME_LEN 8
#define LOG(fmt, ...)      printf("[%-" STR(FUNCTION_NAME_LEN) "." STR(FUNCTION_NAME_LEN) "s] "    \
                fmt "\n", __func__, ##__VA_ARGS__)

#define LOG_RAW(fmt, ...)  printf(fmt, ##__VA_ARGS__)

#define LOG_ERR(fmt, ...)  printf(__FILE_NAME__ ":" STR(__LINE__) "\033[31m"                       \
                "[%-" STR(FUNCTION_NAME_LEN) "." STR(FUNCTION_NAME_LEN) "s] " fmt "\033[0m" "\n",    \
                __func__, ##__VA_ARGS__)
#else

#define FUNCTION_NAME_LEN 0

// 如果未定义 SEG_RTT，全部优化为空，不占用 Flash
#define LOG(fmt, ...)      ((void)0)
#define LOG_RAW(fmt, ...)  ((void)0)
#define LOG_ERR(fmt, ...)    ((void)0)
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
