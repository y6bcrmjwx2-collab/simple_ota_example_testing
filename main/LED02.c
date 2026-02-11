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
#define BLINKS 2

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

// esptool.py -p /dev/tty.usbmodem101 write_flash 0x20000 ./build/LED02.bin

// serial monitor output:
//  the esptool.py write_flash method leads to setting the partition state to VALID.
/*
I (48) boot: Partition Table:
I (50) boot: ## Label            Usage          Type ST Offset   Length
I (56) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (63) boot:  1 otadata          OTA data         01 00 0000f000 00002000
I (70) boot:  2 phy_init         RF data          01 01 00011000 00001000
I (76) boot:  3 ota_0            OTA app          00 10 00020000 001a9000
I (83) boot:  4 ota_1            OTA app          00 11 001d0000 001a9000
I (89) boot: End of partition table
I (92) esp_image: segment 0: paddr=00020020 vaddr=3c020020 size=0a014h ( 40980) map
I (107) esp_image: segment 1: paddr=0002a03c vaddr=3fc92500 size=02ae8h ( 10984) load
I (110) esp_image: segment 2: paddr=0002cb2c vaddr=40374000 size=034ech ( 13548) load
I (118) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=183b0h ( 99248) map
I (140) esp_image: segment 4: paddr=000483d8 vaddr=403774ec size=0af7ch ( 44924) load
I (150) esp_image: segment 5: paddr=0005335c vaddr=50000000 size=00020h (    32) load
I (156) boot: Loaded app from partition at offset 0x20000
I (156) boot: Disabling RNG early entropy source...
I (168) cpu_start: Multicore app
I (177) cpu_start: Pro cpu start user code
I (177) cpu_start: cpu freq: 160000000 Hz
I (177) app_init: Application information:
I (177) app_init: Project name:     LED02
I (181) app_init: App version:      4aa26b6-dirty
I (185) app_init: Compile time:     Feb 11 2026 21:39:19
I (190) app_init: ELF file SHA256:  fd8ffc25b...
I (194) app_init: ESP-IDF:          v5.5.1-dirty
I (199) efuse_init: Min chip rev:     v0.0
I (203) efuse_init: Max chip rev:     v0.99
I (207) efuse_init: Chip rev:         v0.2
I (210) heap_init: Initializing. RAM available for dynamic allocation:
I (217) heap_init: At 3FC958B0 len 00053E60 (335 KiB): RAM
I (222) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (227) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (232) heap_init: At 600FE000 len 00001FE8 (7 KiB): RTCRAM
I (239) spi_flash: detected chip: boya
I (241) spi_flash: flash io: dio
I (244) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (250) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (257) main_task: Started on CPU0
I (267) main_task: Calling app_main()
I (267) APP: Flash LED with OTA partition info.
I (267) APP: === OTA Partition Information ===
I (267) APP: Partition label: ota_0
I (277) APP: Partition type: 0x00
I (277) APP: Partition subtype: 0x10
I (277) APP: Partition address: 0x00020000
I (287) APP: Partition size: 0x1a9000 (1740800 bytes)
I (287) APP: Partition verified: YES
I (297) APP: OTA state: 2
I (297) APP: OTA state: VALID
*/