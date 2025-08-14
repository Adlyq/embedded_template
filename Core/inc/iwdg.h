//
// Created by adlyq on 25-6-20.
//

#pragma once
#include "common_defs.h"
EXTERN_C_BEGIN

/**
 * @brief 初始化独立看门狗
 * @note 配置看门狗超时时间约为260ms
 */
void iwdgInit(void);

EXTERN_C_END
