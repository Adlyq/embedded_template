//
// Created by adlyq on 25-3-26.
//
#include "short_circuit_protection.h"

#include <n32g031.h>
#include <output.h>
#include <util.h>

// 短路保护所需的外设定义
#define RCC_NEED_APB2   (RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO)
#define SCP_PORT        GPIOA
#define SCP_PIN         GPIO_PIN_5
#define SCP_PORT_SOURCE GPIOA_PORT_SOURCE
#define SCP_EXTI_LINE   EXTI_LINE5
#define SCP_PIN_SOURCE  GPIO_PIN_SOURCE5
#define SCP_IRQn        EXTI4_15_IRQn
#define SCP_IRQHandler  EXTI4_15_IRQHandler

#define NUMBER_OF_CONTINUOUS_DETECTIONS 20

/**
 * @brief 检查是否发生短路
 * @note 通过多次连续检测确认短路状态，防止误触发
 */
void checkShort(void) {

    const bool isNoShortCircuit = GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN);
    if (isNoShortCircuit == !isShortCircuit()) return;
    for (int i = 0; i < NUMBER_OF_CONTINUOUS_DETECTIONS; ++i) {
        if (GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN) != isNoShortCircuit) return;

        delayUs(50);
    }

    if (isNoShortCircuit) {
        onNotShortCircuit();
    } else {
        onShortCircuit();
    }
}

/**
 * @brief 初始化短路保护功能
 * @note 配置GPIO
 */
void shortCircuitProtectionInit(void) {
    // 使能所需的时钟
    RCC_EnableAPB2PeriphClk(RCC_NEED_APB2, ENABLE);

    // 配置GPIO为上拉输入
    GPIO_InitType gpioInit;
    GPIO_InitStruct(&gpioInit);
    gpioInit.Pin       = SCP_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_INPUT;
    gpioInit.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(SCP_PORT, &gpioInit);
}
