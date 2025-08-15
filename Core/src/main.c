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
    shortCircuitProtectionInit();
    iwdgInit();
    outputInit();

#ifdef Output_NORMAL_CLOSE
    outputLDSet(true); // 常闭逻辑
#elif defined(Output_NORMAL_OPEN)
    outputLDSet(false); // 常开逻辑
#endif
}

/**
 * @brief 主循环函数
 * @note 处理周期性任务
 */
__STATIC_FORCEINLINE void loop(void) {
    IWDG_ReloadKey();

    // 检查短路状态
    if (checkFlag) {
        checkShort();
    }

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
