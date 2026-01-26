//
// Created by 28331 on 25-2-12.
//
#include "util.h"

#include <stdio.h>

#include "SEGGER_RTT.h"

// 系统时间戳，由SysTick中断更新，单位为毫秒
volatile u32 timestamp     = 0;
static u32   g_random_seed = 0x12345678;

/**
 * @brief 产生毫秒级延时
 * @param ms 延时的毫秒数
 * @retval None
 */
void delayMs(u32 ms) {
    ms = timestamp + ms;
    // 等待直到时间戳达到目标值，同时喂狗防止超时复位
    while ((s32)(ms - timestamp) > 0) {
        IWDG_ReloadKey();
    }
}

/**
 * @brief 产生微秒级延时
 * @param us 延时的微秒数
 * @retval None
 */
void delayUs(u32 us) {
    // 对于大于1000微秒的延时，先使用毫秒延时处理
    if (us >= 1000) {
        delayMs(us / 1000);
        us %= 1000;
    }
    if (us == 0) return;

    // 基于SysTick计数器实现微秒延时
    const u32 start = SysTick->VAL;
    us              *= SystemCoreClock / 1000000; // 将微秒转换为系统时钟周期数
    u32 now;

    // 等待直到经过了指定的时钟周期数
    do {
        now = SysTick->VAL;
    } while (((start >= now) ? (start - now) : (start + (SysTick->LOAD - now))) < us);
}

/**
 * @brief 系统时基初始化，配置SysTick为1ms中断
 * @note 使用constructor属性确保在main函数执行前初始化
 */
__attribute__((constructor(101)))
void sysTimebaseInit(void) {
    // 配置SysTick使用HCLK作为时钟源
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    // 配置SysTick中断周期为1ms
    if (SysTick_Config(SystemCoreClock / 1000)) {
        /* 配置失败，进入死循环 */
        // ReSharper disable once CppDFAEndlessLoop
        while (1);
    }

    // 设置SysTick中断优先级
    // NVIC_SetPriority(SysTick_IRQn, 0);

    g_random_seed = *(uint32_t*)UID_BASE;
    g_random_seed ^= *(uint32_t*)(UID_BASE + 4);
    g_random_seed ^= *(uint32_t*)(UID_BASE + 8);
    g_random_seed ^= SysTick->VAL;

#ifdef SEGGER_RTT
    SEGGER_RTT_Init();

#ifdef STDIO_PRINT
    setvbuf(stdout, NULL, _IOLBF, 128);
#endif

#endif
}

u32 fast_rand(void) {
    g_random_seed = g_random_seed * 1664525 + 1013904223;
    return g_random_seed;
}

/**
 * @brief SysTick中断处理函数
 * @note 每1ms触发一次，更新系统时间戳
 */
void SysTick_Handler(void) {
    timestamp++;
}

/**
 * @brief 初始化定时器输出比较通道
 * @param TIMx 定时器实例
 * @param oc 输出比较通道号(1-4)
 * @param TIM_OCInitStruct 输出比较初始化结构体
 * @retval None
 */
void TIM_InitOc(TIM_Module* TIMx, const uint8_t oc, OCInitType* TIM_OCInitStruct) {
    // 根据通道号调用对应的初始化函数
    switch (oc) {
    case 1:
        TIM_InitOc1(TIMx, TIM_OCInitStruct);
        break;
    case 2:
        TIM_InitOc2(TIMx, TIM_OCInitStruct);
        break;
    case 3:
        TIM_InitOc3(TIMx, TIM_OCInitStruct);
        break;
    case 4:
        TIM_InitOc4(TIMx, TIM_OCInitStruct);
        break;
    default:
        break;
    }
}

#ifdef PALAND_PRINT

void _putchar(const char character) {
    SEGGER_RTT_PutCharSkip(0, character);
}

#endif

#include "cmsis_compiler.h" // 包含 __get_PRIMASK 等定义

// 辅助宏：进入临界区并保存状态
// primask_save 是输出变量，保存当前中断状态
#define ATOMIC_ENTER(primask_save) \
    do { \
    primask_save = __get_PRIMASK(); \
    __disable_irq(); \
    } while(0)

// 辅助宏：退出临界区并恢复状态
#define ATOMIC_EXIT(primask_save) \
    do { \
    __set_PRIMASK(primask_save); \
    } while(0)

unsigned int __atomic_fetch_add_4(volatile void* ptr, const unsigned int val, const int memmodel) {
    (void)memmodel;
    // 旧值
    uint32_t primask_bit;

    ATOMIC_ENTER(primask_bit);

    const unsigned int tmp       = *(volatile unsigned int*)ptr;
    *(volatile unsigned int*)ptr = tmp + val;

    ATOMIC_EXIT(primask_bit);
    return tmp; // 返回旧值
}

unsigned int __atomic_fetch_sub_4(volatile void* ptr, const unsigned int val, const int memmodel) {
    (void)memmodel;
    uint32_t primask_bit;

    ATOMIC_ENTER(primask_bit);

    const unsigned int tmp       = *(volatile unsigned int*)ptr;
    *(volatile unsigned int*)ptr = tmp - val;

    ATOMIC_EXIT(primask_bit);
    return tmp;
}

unsigned int __atomic_load_4(const volatile void* ptr, const int memmodel) {
    (void)memmodel;
    // Cortex-M 上对齐的32位读取天然原子，但为了绝对安全保留锁
    // 如果追求极致速度，且确定 ptr 4字节对齐，可以直接 return *(volatile unsigned int*)ptr;
    uint32_t primask_bit;

    ATOMIC_ENTER(primask_bit);
    const unsigned int tmp = *(volatile unsigned int*)ptr;
    ATOMIC_EXIT(primask_bit);

    return tmp;
}

void __atomic_store_4(volatile void* ptr, const unsigned int val, const int memmodel) {
    (void)memmodel;
    uint32_t primask_bit;

    ATOMIC_ENTER(primask_bit);
    *(volatile unsigned int*)ptr = val;
    ATOMIC_EXIT(primask_bit);
}

_Bool __atomic_compare_exchange_4(volatile void* ptr, void* expected, const unsigned int desired, const _Bool weak,
                                  const int      success_memmodel, const int failure_memmodel) {
    (void)weak;
    (void)success_memmodel;
    (void)failure_memmodel;
    uint32_t primask_bit;
    _Bool    success = 0;

    ATOMIC_ENTER(primask_bit);

    const unsigned int current = *(volatile unsigned int*)ptr;
    if (current == *(unsigned int*)expected) {
        *(volatile unsigned int*)ptr = desired;
        success                      = 1;
    } else {
        // CAS 失败时，必须将当前值写回 expected 指针
        *(unsigned int*)expected = current;
        success                  = 0;
    }

    ATOMIC_EXIT(primask_bit);
    return success;
}
