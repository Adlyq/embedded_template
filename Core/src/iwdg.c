//
// Created by adlyq on 25-6-20.
//

#include "iwdg.h"

#include "gd32f30x.h"

/**
 * @brief 初始化独立看门狗
 * @note 配置看门狗超时时间约为260ms
 */
void fwdgInit(void) {
    /* 启用写入访问 */
    fwdgt_write_enable();

    /* 配置预分频器和重载值
       假设 LSI = 40kHz
       预分频 = 64
       重载 = 165
       溢出时间 = (165+1) * 64 / 40000 ≈ 260ms
    */
    fwdgt_config(165, FWDGT_PSC_DIV64);

    /* 启动看门狗 */
    fwdgt_enable();
}
