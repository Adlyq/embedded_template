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
    // 初始化短路保护功能
    shortCircuitProtectionInit();
    // 初始化独立看门狗
    iwdgInit();
    // 初始化输出控制
    outputInit();

    // 根据编译选项设置输出逻辑方向
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
    // 喂狗，防止系统复位
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
    // 系统初始化
    init();

    // 主循环
    while (true) {
        loop();
    }
}
