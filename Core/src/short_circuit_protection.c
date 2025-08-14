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

// 连续检测次数，用于滤除干扰
#define NUMBER_OF_CONTINUOUS_DETECTIONS 10

// 短路检测标志，由中断设置
bool checkFlag = true;

/**
 * @brief 检查是否发生短路
 * @note 通过多次连续检测确认短路状态，防止误触发
 */
void checkShort(void) {
    checkFlag = false;

    // 读取当前短路检测引脚状态
    const bool isNoShortCircuit = GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN);
    // 连续多次检测确认状态稳定
    for (int i = 0; i < NUMBER_OF_CONTINUOUS_DETECTIONS; ++i) {
        if (GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN) != isNoShortCircuit) return;

        delayUs(15); // 短暂延时
    }

    // 根据检测结果调用相应处理函数
    if (isNoShortCircuit) {
        onNotShortCircuit();
    } else {
        onShortCircuit();
    }
}

/**
 * @brief 初始化短路保护功能
 * @note 配置GPIO和外部中断
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

    // 配置EXTI线路
    GPIO_ConfigEXTILine(SCP_PORT_SOURCE, SCP_PIN_SOURCE);

    // 配置EXTI中断，双边沿触发
    EXTI_InitType extiInit;
    EXTI_InitStruct(&extiInit);
    extiInit.EXTI_Line    = SCP_EXTI_LINE;
    extiInit.EXTI_Mode    = EXTI_Mode_Interrupt;
    extiInit.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    extiInit.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&extiInit);

    // 配置NVIC
    NVIC_InitType nvicInit;
    nvicInit.NVIC_IRQChannel         = SCP_IRQn;
    nvicInit.NVIC_IRQChannelPriority = 0;
    nvicInit.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&nvicInit);
}

/**
 * @brief 短路检测中断处理函数
 * @note 响应EXTI线路中断，设置检测标志
 */
void SCP_IRQHandler() {
    if (EXTI_GetITStatus(SCP_EXTI_LINE)) {
        checkFlag = true; // 设置检测标志，主循环将执行检测

        EXTI_ClrITPendBit(SCP_EXTI_LINE); // 清除中断标志位
    }
}
