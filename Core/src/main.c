/* 包含 FreeRTOS 头 */
#include "FreeRTOS.h"
#include "n32g031.h"
#include "task.h"

/* LED 闪烁任务 */
[[noreturn]] void vLedTask(void* pvParameters) {
    for (;;) {
        /* 切换 PB1 */
        GPIO_TogglePin(GPIOB, GPIO_PIN_1);
        /* 延时 500 ms */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

[[noreturn]] void vRTxTask(void* pvParameters) {
    for (;;) {
        GPIO_TogglePin(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
        /* 这里可以添加 RTOS 相关的代码 */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void) {
    /* 1. 使能 GPIOB 时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_GPIOA, ENABLE);

    /* 2. 配置 PB1 为推挽输出 */
    GPIO_InitType gpio_init;
    GPIO_InitStruct(&gpio_init);
    gpio_init.Pin          = GPIO_PIN_1;
    gpio_init.GPIO_Mode    = GPIO_MODE_OUTPUT_PP;
    gpio_init.GPIO_Speed   = GPIO_SPEED_HIGH;
    gpio_init.GPIO_Current = GPIO_DC_LOW;
    GPIO_InitPeripheral(GPIOB, &gpio_init);

    gpio_init.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitPeripheral(GPIOA, &gpio_init);

    /* 3. 创建 LED 任务 */
    xTaskCreate(
        vLedTask,   /* 任务函数 */
        "LedBlink", /* 任务名 */
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1, /* 任务优先级 */
        NULL
    );

    xTaskCreate(
        vRTxTask,    /* 任务函数 */
        "vRTxBlink", /* 任务名 */
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1, /* 任务优先级 */
        NULL
    );

    /* 4. 启动调度器 */
    vTaskStartScheduler();
}
