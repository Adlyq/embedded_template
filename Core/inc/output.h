//
// Created by adlyq on 25-3-25.
//

#pragma once

#include "common_defs.h"

EXTERN_C_BEGIN
#include <stdbool.h>

// 当前输出状态
extern volatile bool outputting;

/**
 * @brief 初始化输出控制及短路检测引脚
 */
void outputInit(void);

/**
 * @brief 设置输出逻辑方向
 * @param state 逻辑方向状态
 */
void outputLDSet(bool state);

/**
 * @brief 设置/获取逻辑输出状态
 */
void outputSet(bool state);
bool outputGet(void);

/**
 * @brief 检查短路状态 (建议在主循环调用)
 */
void outputCheckShort(void);

/**
 * @brief 获取当前短路状态
 */
bool isShortCircuit(void);

/**
 * @brief LED 闪烁提示及短路恢复逻辑 (建议在短路时调用)
 */
void outputFlash(void);

EXTERN_C_END
