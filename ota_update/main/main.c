#include "driver/uart.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#define BUF_SIZE 1024 // To update
#define TAG "ESP32_MASTER" // To update

//static char ota_write_data[BUF_SIZE + 1] = { 0 };

void ota_task(void *pvParameter)
{
    esp_err_t err;
    esp_ota_handle_t update_handle = 0 ;
    const esp_partition_t *update_partition = NULL;

    ESP_LOGI(TAG, "Starting OTA procedure");

    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();

    if (configured != running) {
        ESP_LOGW(TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x", (unsigned int)configured->address, (unsigned int)running->address);
        ESP_LOGW(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }
    ESP_LOGI(TAG, "Running partition type %d subtype %d (offset 0x%08x)", running->type, running->subtype, (unsigned int)running->address);

    update_partition = esp_ota_get_next_update_partition(NULL);
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x", update_partition->subtype, (unsigned int)update_partition->address);


    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed, error=%d", err);
        return;
    }
    ESP_LOGI(TAG, "esp_ota_begin succeeded");

    char *ota_buff = (char *)malloc(BUF_SIZE);
    if (!ota_buff) {
        ESP_LOGE(TAG, "Memory allocation for firmware update failed");
        return;
    }

    while (1) {
        memset(ota_buff, 0, BUF_SIZE);
        int buff_len = 0; //uart_read_bytes(UART_NUM_1, (uint8_t *)ota_buff, BUF_SIZE, 20 / portTICK_PERIOD_MS); //TO BE REPLACED BY ACTUAL FIRMWARE DATA INPUT
        if (buff_len > 0) {
            err = esp_ota_write( update_handle, (const void *)ota_buff, buff_len);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error: esp_ota_write failed, err=0x%d", err);
                break;
            }
        } else if (buff_len < 0) { //connection closed
            break;
        } else {
            vTaskDelay(1);
        }
    }

    free(ota_buff);
    ota_buff = NULL;


    err = esp_ota_end(update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed");
        return;
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed, err=0x%d", err);
        return;
    }

    /*ESP_LOGI(TAG, "Restarting system");
    esp_restart();*/
    return;
}


void app_main(void)
{
    //NVS initialization
    esp_err_t error = nvs_flash_init();
    if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      error = nvs_flash_init();
    }
    if(error != ESP_OK){
        ESP_LOGE(TAG,"NVS initialization failed");
        return;
    }

    //UART configuration
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);

    //OTA task creation
    error = xTaskCreate(&ota_task, "ota_task", 8192, NULL, 5, NULL);
    if(error != ESP_OK){
        ESP_LOGE(TAG,"OTA task creation failed");
        return;
    }

    while (1) {
        uint8_t data[BUF_SIZE];
        int length = 0;

        //UART receive
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM_0, (size_t*)&length));
        if (length > 0) {
            uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        }

        //UART feedback transmit
        uart_write_bytes(UART_NUM_0, data, BUF_SIZE);
    }
}
