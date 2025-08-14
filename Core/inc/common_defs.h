//
// Created by adlyq on 25-7-21.
//

#pragma once

// C/C++混合编程支持宏
// 用于在C++环境中使用C函数
#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END   }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif
