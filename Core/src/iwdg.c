//
// Created by adlyq on 25-6-20.
//

#include "iwdg.h"

#include "n32g031.h"

/**
 * @brief 初始化独立看门狗
 * @note 配置看门狗超时时间约为260ms
 */
void iwdgInit(void) {
    // 使能PWR时钟，允许在调试模式下暂停IWDG
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    DBG_ConfigPeriph(DBG_IWDG_STOP, ENABLE);

    // 检查并清除IWDG复位标志
    if (RCC_GetFlagStatus(RCC_CTRLSTS_FLAG_IWDGRSTF)) {
        RCC_ClrFlag();
    }

    /* 配置IWDG超时时间约为260ms */
    // 使能对IWDG_PR和IWDG_RLR寄存器的写访问
    IWDG_WriteConfig(IWDG_WRITE_ENABLE);

    // 设置IWDG预分频为128
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV128);

    /* 设置重载值，计算方法：
       超时时间 = 重载值 * (LSI周期 * 预分频)
       260ms = 61 * (1/40000 * 128)
     */
    IWDG_CntReload(61);

    // 重载IWDG计数器
    IWDG_ReloadKey();

    // 使能IWDG(LSI振荡器会由硬件自动使能)
    IWDG_Enable();
}
