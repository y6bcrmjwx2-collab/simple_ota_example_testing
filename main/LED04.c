#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "esp_image_format.h"

#define BUILT_IN_LED 1
#define ON 1
#define OFF 0
#define BLINKS 4

static const char *TAG = "APP";

void print_running_partition_info(void)
{
    const esp_partition_t *running_partition = esp_ota_get_running_partition(); // pointer to info for partition which should be updated next

    if (running_partition == NULL)
    {
        ESP_LOGE(TAG, "Failed to get running partition");
        return;
    }

    // partition info
    ESP_LOGI(TAG, "=== OTA Partition Information ===");
    ESP_LOGI(TAG, "Partition label: %s", running_partition->label);
    ESP_LOGI(TAG, "Partition type: 0x%02x", running_partition->type);
    ESP_LOGI(TAG, "Partition subtype: 0x%02x", running_partition->subtype);
    ESP_LOGI(TAG, "Partition address: 0x%08" PRIx32, running_partition->address);
    ESP_LOGI(TAG, "Partition size: 0x%" PRIx32 " (%" PRIu32 " bytes)",
             running_partition->size, running_partition->size);

    // from esp_partition.h
    const esp_partition_t *verified_partition = esp_partition_verify(running_partition); // returns a pointer to the esp_partition_t structure in flash
    if (verified_partition != NULL)
    {
        ESP_LOGI(TAG, "Partition verified: YES");
    }
    else
    {
        ESP_LOGI(TAG, "Partition verified: NO");
    }

    // esp_partition.h
    // uint8_t sha256[32];
    // esp_err_t ret = esp_partition_get_sha256(running_partition, sha256);
    // if (ret == ESP_OK) {
    //     ESP_LOGI(TAG, "Partition SHA256:");
    //     ESP_LOG_BUFFER_HEXDUMP(TAG, sha256, sizeof(sha256), ESP_LOG_INFO);
    // } else {
    //     ESP_LOGE(TAG, "Failed to get SHA256: %s", esp_err_to_name(ret));
    // }

    // get ota state 来自 esp_ota_ops.h
    if (running_partition->type == ESP_PARTITION_TYPE_APP)
    {

        esp_ota_img_states_t ota_state;
        esp_err_t ret = esp_ota_get_state_partition(running_partition, &ota_state);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "OTA state: %d", ota_state);
            switch (ota_state)
            {
            case ESP_OTA_IMG_NEW:
                ESP_LOGI(TAG, "OTA state: NEW");
                break;
            case ESP_OTA_IMG_PENDING_VERIFY:
                ESP_LOGI(TAG, "OTA state: PENDING_VERIFY");
                break;
            case ESP_OTA_IMG_VALID:
                ESP_LOGI(TAG, "OTA state: VALID");
                break;
            case ESP_OTA_IMG_INVALID:
                ESP_LOGI(TAG, "OTA state: INVALID");
                break;
            case ESP_OTA_IMG_ABORTED:
                ESP_LOGI(TAG, "OTA state: ABORTED");
                break;
            default:
                ESP_LOGI(TAG, "OTA state: UNDEFINED");
                break;
            }
        }
    }
}

void app_main(void)
{
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "Flash LED with OTA partition info.");
    print_running_partition_info();

    while (1)
    {

        int i;
        for (i = 0; i < BLINKS; i++)
        {
            gpio_set_level(BUILT_IN_LED, ON);
            vTaskDelay(30);
            gpio_set_level(BUILT_IN_LED, OFF);
            vTaskDelay(30);
        }

        gpio_set_level(BUILT_IN_LED, OFF);
    vTaskDelay(500);
    }
}

// esptool.py -p /dev/tty.usbmodem101 write_flash 0x1d0000 ./build/LED04.bin