#include "main.h"

#include <stdio.h>

#include "short_circuit_protection.h"
#include "iwdg.h"
#include "output.h"
#include "util.h"

/**
 * @brief 系统初始化函数
 * @note 初始化各个模块
 */
__STATIC_FORCEINLINE void init(void) {
#ifdef LOCK_MCU
    FLASH_Unlock();
    FLASH_ReadOutProtectionL1(ENABLE);
    FLASH_GetReadOutProtectionSTS();
    FLASH_Lock();
#endif
    LOG("System Init");
    shortCircuitProtectionInit();
    iwdgInit();
    outputInit();

#ifdef Output_LIGHT_CLOSE
    outputLDSet(false); // 遮光ON
#elif defined(Output_LIGHT_OPEN)
    outputLDSet(true); // 入光ON
#endif
}

/**
 * @brief 主循环函数
 * @note 处理周期性任务
 */
__STATIC_FORCEINLINE void loop(void) {
    IWDG_ReloadKey();

    // 检查短路状态
    checkShort();

    // 短路状态下执行LED闪烁提示
    if (isShortCircuit()) {
        outputFlash();
    }

    delayMs(1000);
}

/**
 * @brief 主函数
 * @return 不会返回
 */
__NO_RETURN int main(void) {
    init();
    delayMs(50);
    LOG("System Start");
    while (true) {
        loop();
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(const uint8_t* expr, const uint8_t* file, const uint32_t line) {
    (void)expr;
    (void)file;
    (void)line;

    __BKPT();
    /* 无限循环以便调试 */
    // ReSharper disable once CppDFAEndlessLoop
    while (true) {
    }
}
#endif