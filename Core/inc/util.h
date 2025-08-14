//
// Created by 28331 on 25-2-12.
//

#pragma once

#include "common_defs.h"

EXTERN_C_BEGIN

#include <stdint.h>
#include <n32g031_tim.h>
#include <stdlib.h>

// 等待某个条件满足的宏
#define LOOP_UNTIL(condition) while (!(condition)) { }
/**
 * @brief 返回两个值中的较大值
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
/**
 * @brief 返回两个值中的较小值
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
/**
 * @brief 判断两个值是否近似相等(差值不超过EPSILON)
 */
#define APPROX_EQUAL(a, b, EPSILON) (abs((int32_t)(a) - (int32_t)(b)) <= (EPSILON))

/**
 * @brief 优化标记，用于提高性能关键代码的执行效率
 */
#if __has_attribute(optimize)
#define OPTIMIZE_O3 __attribute__((optimize("O3", "unroll-loops")))
#else
#define OPTIMIZE_O3
#endif

/**
 * @brief 类型定义，用于简化代码
 */
typedef uint64_t u64;
typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;

/**
 * @brief 系统时间戳，由SysTick中断更新，单位为毫秒
 */
extern volatile u32 timestamp;

/**
 * @brief 毫秒级延时函数
 * @param ms 延时的毫秒数
 */
void delayMs(uint32_t ms);

/**
 * @brief 微秒级延时函数
 * @param us 延时的微秒数
 */
void delayUs(u32 us);

/**
 * @brief 初始化定时器输出比较通道
 * @param TIMx 定时器实例
 * @param oc 输出比较通道号(1-4)
 * @param TIM_OCInitStruct 输出比较初始化结构体
 */
void TIM_InitOc(TIM_Module* TIMx, uint8_t oc, OCInitType* TIM_OCInitStruct);

EXTERN_C_END
