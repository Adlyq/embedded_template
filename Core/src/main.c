#include "main.h"

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
}

/**
 * @brief 主函数
 * @return 不会返回
 */
__NO_RETURN int main(void) {
    init();

    while (true) {
        loop();
    }
}
