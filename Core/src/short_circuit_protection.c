//
// Created by adlyq on 25-3-26.
//
#include "short_circuit_protection.h"

#include <n32g031.h>
#include <output.h>
#include <util.h>

#define RCC_NEED_APB2   (RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO)
#define SCP_PORT        GPIOA
#define SCP_PIN         GPIO_PIN_5
#define SCP_PORT_SOURCE GPIOA_PORT_SOURCE
#define SCP_EXTI_LINE   EXTI_LINE5
#define SCP_PIN_SOURCE  GPIO_PIN_SOURCE5
#define SCP_IRQn        EXTI4_15_IRQn
#define SCP_IRQHandler  EXTI4_15_IRQHandler

#define NUMBER_OF_CONTINUOUS_DETECTIONS 10

bool checkFlag = true;

void checkShort(void) {
    checkFlag = false;

    const bool isNoShortCircuit = GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN);
    for (int i = 0; i < NUMBER_OF_CONTINUOUS_DETECTIONS; ++i) {
        if (GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN) != isNoShortCircuit) return;

        delayUs(15);
    }
    if (isNoShortCircuit) {
        onNotShortCircuit();
    } else {
        onShortCircuit();
    }
}

void shortCircuitProtectionInit(void) {
    RCC_EnableAPB2PeriphClk(RCC_NEED_APB2, ENABLE);

    GPIO_InitType gpioInit;
    GPIO_InitStruct(&gpioInit);
    gpioInit.Pin       = SCP_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_INPUT;
    gpioInit.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(SCP_PORT, &gpioInit);

    GPIO_ConfigEXTILine(SCP_PORT_SOURCE, SCP_PIN_SOURCE);

    EXTI_InitType extiInit;
    EXTI_InitStruct(&extiInit);
    extiInit.EXTI_Line    = SCP_EXTI_LINE;
    extiInit.EXTI_Mode    = EXTI_Mode_Interrupt;
    extiInit.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    extiInit.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&extiInit);

    NVIC_InitType nvicInit;
    nvicInit.NVIC_IRQChannel         = SCP_IRQn;
    nvicInit.NVIC_IRQChannelPriority = 0;
    nvicInit.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&nvicInit);
}

void SCP_IRQHandler() {
    if (EXTI_GetITStatus(SCP_EXTI_LINE)) {
        checkFlag = true;

        EXTI_ClrITPendBit(SCP_EXTI_LINE);
    }
}
