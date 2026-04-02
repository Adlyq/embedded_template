//
// Created by adlyq on 25-3-25.
//

#include "output.h"

#include <n32g031.h>
#include <stdbool.h>
#include <util.h>

/**
 * @brief 输出控制及短路保护所需的外设定义
 */
#define RCC_NEED_APB2       (RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO)

#define OUTPUT_LED_PORT      GPIOA
#define OUTPUT_LED_PIN       GPIO_PIN_4
#define OUTPUT_LO_PORT       GPIOB // 黑色引脚
#define OUTPUT_LO_PIN        GPIO_PIN_1
#define OUTPUT_DO_PORT       GPIOB // 白色引脚
#define OUTPUT_DO_PIN        GPIO_PIN_6

#define SCP_PORT             GPIOA
#define SCP_PIN              GPIO_PIN_5

// 当前短路标志
static bool shortFlag = false;
// 恢复计数器 (0表示正常运行)
static uint8_t recoveryCounter = 0;
// 逻辑输出电平
volatile bool outputting = false;
// 逻辑方向
static bool ld = false;

/**
 * @brief 内部函数：物理硬件输出控制
 */
static void setHardwareOutput(bool state) {
    if (state) {
        GPIO_SetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
        GPIO_SetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);
#ifdef OUTPUT_DO_PORT
        GPIO_ResetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);
#endif
    } else {
        GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
        GPIO_ResetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);
#ifdef OUTPUT_DO_PORT
        GPIO_SetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);
#endif
    }
}

void outputInit(void) {
    RCC_EnableAPB2PeriphClk(RCC_NEED_APB2, ENABLE);

    GPIO_InitType gpioInit;
    GPIO_InitStruct(&gpioInit);

    // 配置输出引脚 (LED, LO, DO)
    gpioInit.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.GPIO_Pull = GPIO_PULL_DOWN;
    gpioInit.Pin       = OUTPUT_LED_PIN;
    GPIO_InitPeripheral(OUTPUT_LED_PORT, &gpioInit);

    gpioInit.Pin = OUTPUT_LO_PIN;
    GPIO_InitPeripheral(OUTPUT_LO_PORT, &gpioInit);

#ifdef OUTPUT_DO_PORT
    gpioInit.Pin = OUTPUT_DO_PIN;
    GPIO_InitPeripheral(OUTPUT_DO_PORT, &gpioInit);
#endif

    // 配置短路检测引脚 (上拉输入)
    gpioInit.Pin       = SCP_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_INPUT;
    gpioInit.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(SCP_PORT, &gpioInit);
}

void outputLDSet(const bool state) {
    const bool new_ld = !state;
    if (new_ld != ld) outputSet(outputting); // 更新输出状态以反映新的逻辑方向
    ld = new_ld;
}

void outputSet(const bool state) {
    if (outputting == state) return;
    outputting = state;
    if (shortFlag) return;

#ifdef OUTPUT_DO_PORT
    setHardwareOutput(state);
#else
    setHardwareOutput(state ^ ld);
#endif
}

bool outputGet(void) {
    return outputting;
}

/**
 * @brief 检测引脚电平。如果是低电平，并经过多次连续确认，则立即设置短路标志。
 */
void outputCheckShort(void) {
    // 硬件初步检测到低电平 (短路)
    if (GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN) == Bit_RESET) {
        // 多次连续检测防误判
        for (int i = 0; i < 20; ++i) {
            if (GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN) != Bit_RESET) return;
            delayUs(50);
        }

        if (!shortFlag) {
            shortFlag = true;
            setHardwareOutput(false); // 立即切断输出
        }
        recoveryCounter = 5; // 重置恢复倒计时
    }
}

bool isShortCircuit(void) {
    return shortFlag;
}

/**
 * @brief 短路时闪烁提示，并根据物理状态递减恢复计数。
 */
void outputFlash(void) {
    static uint32_t nextToggle = 0;
    if ((int32_t)(timestamp - nextToggle) < 0) return;
    nextToggle = timestamp + 200;

    // 如果初步读取到物理电平已恢复为高 (不短路了)
    if (GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN) == Bit_SET) {
        // 恢复时同样进行多次检测防误判
        for (int i = 0; i < 20; ++i) {
            if (GPIO_ReadInputDataBit(SCP_PORT, SCP_PIN) != Bit_SET) {
                recoveryCounter = 5;
                return;
            }
            delayUs(50);
        }

        if (recoveryCounter > 0) recoveryCounter--;
        else shortFlag = false; // 稳定恢复，清除短路标志
    } else {
        recoveryCounter = 5; // 如果物理电平依然是低，强制重置倒计时
    }

    GPIO_TogglePin(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
}

