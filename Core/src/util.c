//
// Created by 28331 on 25-2-12.
//
#include "util.h"

/**
 * @brief 产生延时
 * @param  nCount: time = nCount * 4 / SystemCoreClock;
 * @retval None
 */
void SoftDelay(volatile uint32_t nCount) {
    while (nCount--) {
        IWDG_ReloadKey();
    }
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
