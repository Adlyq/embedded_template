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
#define OUTPUT_LO_PORT       GPIOB
#define OUTPUT_LO_PIN        GPIO_PIN_1

#ifdef OUTPUT_DEST_L
#defind LED_OUTPUT() GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN)
#define LED_NO_OUTPUT() GPIO_SetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN)
#else
#define LED_OUTPUT() GPIO_SetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN)
#define LED_NO_OUTPUT() GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN)
#endif

static u8   shortCircuit = 0;
static bool flag         = false;
static bool ld           = false;
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
}

void outputLDSet(const bool state) {
    ld = state;
}

void outputSet(const bool state) {
    if (shortCircuit != 0) return;
    outputting = state;
    if (state ^ ld) {
        LED_OUTPUT();
        GPIO_SetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN); // LED
    } else {
        LED_NO_OUTPUT();
        GPIO_ResetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN); // LED
    }
}

bool outputGet(void) {
    return outputting;
}

void onShortCircuit() {
    shortCircuit = 5;
    flag         = true;
    GPIO_ResetBits(OUTPUT_LED_PORT, OUTPUT_LED_PIN); // LED
    GPIO_ResetBits(OUTPUT_LO_PORT, OUTPUT_LO_PIN);   // LO
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
    SoftDelay(400000);
    if (shortCircuit == 0) return;
    GPIO_TogglePin(OUTPUT_LED_PORT, OUTPUT_LED_PIN); // LED
    SoftDelay(400000);
}
