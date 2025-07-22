//
// Created by 28331 on 25-2-12.
//
#include "util.h"

volatile u32 timestamp = 0;

/**
 * @brief 产生延时
 * @param ms 延时的毫秒数
 * @retval None
 */
void delayMs(u32 ms) {
    ms = timestamp + ms;
    while ((s32)(ms - timestamp) > 0) {
        IWDG_ReloadKey();
    }
}

void delayUs(u32 us) {
    if (us >= 1000) {
        delayMs(us / 1000);
        us %= 1000;
    }
    if (us == 0) return;

    const u32 start = SysTick->VAL;
    us *= SystemCoreClock / 1000000;
    u32       now;

    do {
        now = SysTick->VAL;
    } while (((start >= now) ? (start - now) : (start + (SysTick->LOAD - now))) < us);
}

__attribute__((constructor(101)))
void sysTimebaseInit(void) {
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    if (SysTick_Config(SystemCoreClock / 1000)) {
        /* Capture error */
        while (1);
    }

    // Set the priority of the SysTick interrupt
    // NVIC_SetPriority(SysTick_IRQn, 0);
}

/**
 * @brief  This function handles SysTick Handler.
 */
void SysTick_Handler(void) {
    timestamp++;
}

void TIM_InitOc(TIM_Module* TIMx, const uint8_t oc, OCInitType* TIM_OCInitStruct) {
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
