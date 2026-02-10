#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUILT_IN_LED 1
#define ON 1
#define OFF 0
#define BLINKS 4

void app_main(void)
{
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);

    ESP_LOGI("APP", "Flash LED.");

    while (1)
    {

        int i;
        for (i = 0; i < BLINKS; i++)
        {
            gpio_set_level(BUILT_IN_LED, ON);
            vTaskDelay(25);
            gpio_set_level(BUILT_IN_LED, OFF);
            vTaskDelay(25);
        }

        gpio_set_level(BUILT_IN_LED, OFF);
        vTaskDelay(200);
    }
}