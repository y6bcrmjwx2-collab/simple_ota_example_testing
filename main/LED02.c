#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUILT_IN_LED GPIO_NUM_1
#define ON 1
#define OFF 0
#define BLINKS 2

static const char *TAG = "APP";

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUILT_IN_LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    ESP_LOGI(TAG, "Flash LED.");

    while (1)
    {

        int i;
        for (i = 0; i < BLINKS; i++)
        {
            gpio_set_level(BUILT_IN_LED, ON);
            vTaskDelay(pdMS_TO_TICKS(25));
            gpio_set_level(BUILT_IN_LED, OFF);
            vTaskDelay(pdMS_TO_TICKS(25));
        }

        gpio_set_level(BUILT_IN_LED, OFF);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

// esptool.py -p /dev/tty.usbmodem101 write_flash 0x20000 ./build/LED02.bin