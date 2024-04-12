| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# OTA functionality project

Project consists of serial connection between the board and the firmware host, as well as CAN bus between the boards themselves, to enable reliable transfer of data.

## Serial

Basic UART receive implemented.
Basic UART feedback transmit implemented.

## CAN

CAN omitted.

## OTA 

Basic OTA task implemented. Need to revise. Logs doesn't show it, but possible stack overflow in the future.
