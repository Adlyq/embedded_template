//
// Created by adlyq on 25-3-26.
//
#pragma once

#include "common_defs.h"

EXTERN_C_BEGIN

#include <stdbool.h>

// 短路检测标志，由中断设置
// 为true时表示需要执行短路检测
extern bool checkFlag;

/**
 * @brief 检查是否发生短路
 * @note 通过多次连续检测确认短路状态，防止误触发
 */
void checkShort(void);

/**
 * @brief 初始化短路保护功能
 * @note 配置GPIO和外部中断
 */
void shortCircuitProtectionInit(void);

EXTERN_C_END
