#include "driver/uart.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUF_SIZE (1024) // To update

void app_main(void)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);


    while (1) {
        uint8_t data[BUF_SIZE];
        int length = 0;
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM_0, (size_t*)&length));
        if (length > 0) {
            uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        }
        uart_write_bytes(UART_NUM_0, data, BUF_SIZE);
    }
}
