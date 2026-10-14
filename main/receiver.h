#ifndef RECEIVER_H
#define RECEIVER_H

#include "driver/uart.h"
#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#define IBUS_UART          UART_NUM_1

#define IBUS_TX_PIN        UART_PIN_NO_CHANGE
#define IBUS_RX_PIN        16

#define IBUS_BAUDRATE      115200

typedef struct
{
    uint16_t ch1;
    uint16_t ch2;
    uint16_t ch3;
    uint16_t ch4;
    uint16_t ch5;
    uint16_t ch6;
} receiver_t;

esp_err_t receiver_init(void);
esp_err_t receiver_update(void);

uint16_t receiver_get_roll(void);
uint16_t receiver_get_pitch(void);
uint16_t receiver_get_throttle(void);
uint16_t receiver_get_yaw(void);
uint16_t receiver_get_aux1(void);
uint16_t receiver_get_aux2(void);

#endif