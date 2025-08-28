//
// Created by adlyq on 25-3-25.
//

#include "output.h"

#include <n32g031.h>
#include <stdbool.h>
#include <util.h>

/**
 * @brief 输出控制所需的外设定义
 */
#define RCC_NEED_APB2       (RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB)

#define OUTPUT_LED_PORT      GPIOA
#define OUTPUT_LED_PIN       GPIO_PIN_4
#define OUTPUT_LO_PORT       GPIOB // 黑色引脚
#define OUTPUT_LO_PIN        GPIO_PIN_1
#define OUTPUT_DO_PORT       GPIOB // 白色引脚
#define OUTPUT_DO_PIN        GPIO_PIN_6

// 短路计数器，用于短路恢复
static u8 shortCircuit = 0;
// 短路标志
static bool flag = false;
// 当前输出状态
volatile bool outputting = false;

/**
 * @brief 初始化输出控制引脚
 */
void outputInit(void) {
    // 使能所需的时钟
    RCC_EnableAPB2PeriphClk(RCC_NEED_APB2, ENABLE);

    // 配置LED输出引脚
    GPIO_InitType gpioInit;
    GPIO_InitStruct(&gpioInit);
    gpioInit.Pin       = OUTPUT_LED_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.GPIO_Pull = GPIO_PULL_DOWN;
    GPIO_InitPeripheral(OUTPUT_LED_PORT, &gpioInit);

    // 配置LO输出引脚
    GPIO_InitStruct(&gpioInit);
    gpioInit.Pin       = OUTPUT_LO_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.GPIO_Pull = GPIO_PULL_DOWN;
    GPIO_InitPeripheral(OUTPUT_LO_PORT, &gpioInit);

#ifdef OUTPUT_DO_PORT
    // 配置DO输出引脚(如果启用)
    GPIO_InitStruct(&gpioInit);
    gpioInit.Pin       = OUTPUT_DO_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.GPIO_Pull = GPIO_PULL_DOWN;
    GPIO_InitPeripheral(OUTPUT_DO_PORT, &gpioInit);
#endif
}

/**
 * @brief 逻辑反转状态
 */
static bool ld = false;

/**
 * @brief 设置输出逻辑方向
 * @param state 逻辑方向状态
 */
void outputLDSet(const bool state) {
    ld = !state;
}

/**
 * @brief 设置输出状态
 * @param state 输出状态(true:开启, false:关闭)
 * @note 考虑逻辑方向和短路保护
 */
void outputSet(const bool state) {
    // 短路状态下不执行输出
    if (shortCircuit != 0) return;
    outputting = state; // 记录当前输出状态

    // 根据逻辑方向决定实际输出状态
#ifdef OUTPUT_DO_PORT
    if (state) {
#else
    if (state ^ ld) {
#endif
        GPIO_SetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);
#ifdef OUTPUT_DO_PORT
        GPIO_SetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);
#endif
    } else {
        GPIO_ResetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);
#ifdef OUTPUT_DO_PORT
        GPIO_ResetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);
#endif
    }

    // 始终根据实际状态控制LED(如果定义了保持状态)
    if (state ^ ld) {
        GPIO_SetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
    } else {
        GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
    }
}

/**
 * @brief 短路事件处理函数
 * @note 设置短路计数，关闭所有输出
 */
void onShortCircuit() {
    shortCircuit = 5; // 设置短路恢复计数
    flag         = true;
    // 关闭所有输出
    GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
    GPIO_ResetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);
#ifdef OUTPUT_DO_PORT
    GPIO_ResetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);
#endif
}

/**
 * @brief 非短路事件处理函数
 */
void onNotShortCircuit() {
    flag = false;
}

/**
 * @brief 获取当前短路状态
 * @return 是否处于短路状态
 */
bool isShortCircuit() {
    return shortCircuit != 0;
}

/**
 * @brief LED闪烁函数，用于短路提示
 * @note 同时递减短路计数器
 */
void outputFlash(void) {
    if (shortCircuit > flag) shortCircuit--;

    GPIO_TogglePin(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
    delayMs(200);
    GPIO_TogglePin(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
    delayMs(200);
}
