#include "main.h"

#include <stdbool.h>

#include "gd32f30x_fwdgt.h"
#include "iwdg.h"
#include "util.h"

/**
 * @brief 系统初始化函数
 * @note 初始化各个模块
 */
__STATIC_FORCEINLINE void init(void) {
    fwdgInit();
}

/**
 * @brief 主循环函数
 * @note 处理周期性任务
 */
__STATIC_FORCEINLINE void loop(void) {
    fwdgt_counter_reload();
}

/**
 * @brief 主函数
 * @return 不会返回
 */
[[noreturn]] int main(void) {
    init();

    while (true) {
        loop();
    }
}
