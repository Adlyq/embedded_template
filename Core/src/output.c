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
#define OUTPUT_LO_PORT       GPIOA
#define OUTPUT_LO_PIN        GPIO_PIN_9
#define OUTPUT_DO_PORT       GPIOB
#define OUTPUT_DO_PIN        GPIO_PIN_6

#ifdef OUTPUT_DEST_L
#defind LED_OUTPUT() GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN)
#define LED_NO_OUTPUT() GPIO_SetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN)
#else
#define LED_OUTPUT() GPIO_SetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN)
#define LED_NO_OUTPUT() GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN)
#endif

static volatile u8 shortCircuit = 0;
static bool        flag         = false;

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

    GPIO_InitStruct(&gpioInit);
    gpioInit.Pin       = OUTPUT_DO_PIN;
    gpioInit.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.GPIO_Pull = GPIO_PULL_DOWN;
    GPIO_InitPeripheral(OUTPUT_DO_PORT, &gpioInit);
}

void outputStart(void) {
    if (shortCircuit != 0) return;
    LED_OUTPUT();
    GPIO_SetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);   // LO
    GPIO_ResetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN); // DO
}

void outputStop(void) {
    if (shortCircuit != 0) return;
    LED_NO_OUTPUT();
    GPIO_ResetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN); // LO
    GPIO_SetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);   // DO
}

bool outputGet(void) {
    return GPIO_ReadOutputDataBit(OUTPUT_LED_PORT, OUTPUT_LED_PIN);
}

void onShortCircuit() {
    shortCircuit = 5;
    GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN); // LED
    GPIO_ResetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);   // LO
    GPIO_ResetBits(OUTPUT_DO_PORT, OUTPUT_DO_PIN);   // DO
}

void onNotShortCircuit() {
    if (shortCircuit != 0) {
        flag = true;
    } else {
        shortCircuit = false;
        flag         = false;
    }
}

bool isShortCircuit() {
    return shortCircuit != 0;
}

void outputFlash(void) {
    if (shortCircuit == 0 || --shortCircuit == 0) {
        flag = false;
    }
    if (shortCircuit == 0) return;
    GPIO_TogglePin(OUTPUT_LED_PORT, OUTPUT_LED_PIN); // LED
    SoftDelay(400000);
    if (shortCircuit == 0) return;
    GPIO_TogglePin(OUTPUT_LED_PORT, OUTPUT_LED_PIN); // LED
    SoftDelay(400000);
}
