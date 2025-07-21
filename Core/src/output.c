//
// Created by adlyq on 25-3-25.
//

#include "output.h"

#include <n32g031.h>
#include <stdbool.h>
#include <util.h>

#define RCC_NEED_APB2       (RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB)

#define OUTPUT_LED_PORT      GPIOA
#define OUTPUT_LED_PIN       GPIO_PIN_4
#define OUTPUT_LO_PORT       GPIOB // 黑
#define OUTPUT_LO_PIN        GPIO_PIN_1
#define OUTPUT_DO_PORT       GPIOB // 白
#define OUTPUT_DO_PIN        GPIO_PIN_6

static u8   shortCircuit = 0;
static bool flag         = false;
static bool outputting   = false;

void outputInit(void) {
    RCC_EnableAPB2PeriphClk(RCC_NEED_APB2, ENABLE);

    GPIO_InitType gpioInit;
    GPIO_InitStruct(&gpioInit);
    gpioInit.Pin       = OUTPUT_LED_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.GPIO_Pull = GPIO_PULL_DOWN;
    GPIO_InitPeripheral(OUTPUT_LED_PORT, &gpioInit);

    GPIO_InitStruct(&gpioInit);
    gpioInit.Pin       = OUTPUT_LO_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.GPIO_Pull = GPIO_PULL_DOWN;
    GPIO_InitPeripheral(OUTPUT_LO_PORT, &gpioInit);

#ifdef OUTPUT_DO_PORT
    GPIO_InitStruct(&gpioInit);
    gpioInit.Pin       = OUTPUT_DO_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.GPIO_Pull = GPIO_PULL_DOWN;
    GPIO_InitPeripheral(OUTPUT_DO_PORT, &gpioInit);
#endif
}

#ifdef OUTPUT_DEST_SUPPORT
static bool ld = false;

void outputLDSet(const bool state) {
    ld = !state;
}

void outputSet(const bool state) {
    if (shortCircuit != 0) return;
    outputting = state;
    if (state ^ ld) {
#ifndef OUTPUT_DEST_KEEP
        GPIO_SetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
#endif
        GPIO_SetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);
#ifdef OUTPUT_DO_PORT
        GPIO_SetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);
#endif
    } else {
#ifndef OUTPUT_DEST_KEEP
        GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
#endif
        GPIO_ResetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);
#ifdef OUTPUT_DO_PORT
        GPIO_ResetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);
#endif
    }

#ifdef OUTPUT_DEST_KEEP
    if (state) {
        GPIO_SetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
    } else {
        GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
    }
#endif
}
#else
void outputSet(const bool state) {
    if (shortCircuit != 0) return;
    outputting = state;
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
#endif

bool outputGet(void) {
    return outputting;
}

void onShortCircuit() {
    shortCircuit = 5;
    flag         = true;
    GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN); // LED
    GPIO_ResetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);   // LO
#ifdef OUTPUT_DO_PORT
    GPIO_ResetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);
#endif
}

void onNotShortCircuit() {
    flag = false;
}

bool isShortCircuit() {
    return shortCircuit != 0;
}

void outputFlash(void) {
    if (shortCircuit > flag) shortCircuit--;

    if (shortCircuit == 0) return;
    GPIO_TogglePin(OUTPUT_LED_PORT, OUTPUT_LED_PIN); // LED
    SoftDelay(100000);
    if (shortCircuit == 0) return;
    GPIO_TogglePin(OUTPUT_LED_PORT, OUTPUT_LED_PIN); // LED
    SoftDelay(100000);
}
