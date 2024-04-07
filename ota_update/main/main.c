#include "driver/uart.h"
#include "driver/twai.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUF_SIZE 1024 // To update
#define CAN_RX_IO 18
#define CAN_TX_IO 19

void app_main(void)
{
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

    //CAN configuration
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_IO, CAN_RX_IO, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS(); //To adjust
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) twai_start();



    while (1) {
        uint8_t data[BUF_SIZE];
        int length = 0;

        //UART receive
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM_0, (size_t*)&length));
        if (length > 0) {
            uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        }

        //CAN forwarding
        if (length > 0) {
            twai_message_t message;
            message.identifier = 0x100; // To adjust dynamically
            message.data_length_code = length;
            memcpy(message.data, data, length);
            twai_transmit(&message, pdMS_TO_TICKS(10));
        }

        //UART feedback transmit
        uart_write_bytes(UART_NUM_0, data, BUF_SIZE);
    }
}
