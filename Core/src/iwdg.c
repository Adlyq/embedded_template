//
// Created by adlyq on 25-6-20.
//

#include "iwdgInit.h"

#include "n32g031.h"

void iwdgInit(void) {
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    DBG_ConfigPeriph(DBG_IWDG_STOP, ENABLE);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    if (SysTick_Config(SystemCoreClock / 1000)) {
        /* Capture error */
        while (1);
    }

    if (RCC_GetFlagStatus(RCC_CTRLSTS_FLAG_IWDGRSTF)) {
        RCC_ClrFlag();
    }

    /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
       dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteConfig(IWDG_WRITE_ENABLE);

    /* IWDG counter clock: LSI/128 */
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV128);

    /* Set counter reload value to obtain 250ms IWDG TimeOut.
       Counter Reload Value = 250ms/IWDG counter clock period
                            = 250ms / (LSI/128)
                            = 4.27ms / (LsiFreq/128)
     */
    /* 260ms */
    IWDG_CntReload(61);
    /* Reload IWDG counter */
    IWDG_ReloadKey();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}
