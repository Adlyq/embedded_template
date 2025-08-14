//
// Created by adlyq on 25-3-25.
//

#pragma once

#include "common_defs.h"

EXTERN_C_BEGIN
#include <stdbool.h>

// 启用逻辑方向控制支持
#define OUTPUT_DEST_SUPPORT
// #define OUTPUT_DEST_KEEP

// 当前输出状态
extern volatile bool outputting;

/**
 * @brief 初始化输出控制引脚
 */
void outputInit(void);

#ifdef OUTPUT_DEST_SUPPORT
/**
 * @brief 设置输出逻辑方向
 * @param state 逻辑方向状态(true:正向, false:反向)
 */
void outputLDSet(bool);
#endif

/**
 * @brief 设置输出状态
 * @param state 输出状态(true:开启, false:关闭)
 */
void outputSet(bool);

/**
 * @brief 获取当前输出状态
 * @return 当前输出状态
 */
bool outputGet(void);

/**
 * @brief 短路事件处理函数
 * @note 设置短路计数，关闭所有输出
 */
void onShortCircuit();

/**
 * @brief 非短路事件处理函数
 */
void onNotShortCircuit();

/**
 * @brief 获取当前短路状态
 * @return 是否处于短路状态
 */
bool isShortCircuit();

/**
 * @brief LED闪烁函数，用于短路提示
 */
void outputFlash(void);

EXTERN_C_END
