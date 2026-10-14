#include "receiver.h"

#define IBUS_PACKET_SIZE    32

static uint8_t ibus_packet[IBUS_PACKET_SIZE];
static receiver_t rx;

static bool ibus_checksum(uint8_t *packet)
{
    uint16_t checksum = 0xFFFF;

    for(int i = 0; i < 30; i++)
        checksum -= packet[i];

    uint16_t received =
        packet[30] |
        (packet[31] << 8);

    return (checksum == received);
}

esp_err_t receiver_init(void)
{
    uart_config_t uart_config =
    {
        .baud_rate = IBUS_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    ESP_ERROR_CHECK(
        uart_driver_install(
            IBUS_UART,
            1024,
            0,
            0,
            NULL,
            0));

    ESP_ERROR_CHECK(
        uart_param_config(
            IBUS_UART,
            &uart_config));

    ESP_ERROR_CHECK(
        uart_set_pin(
            IBUS_UART,
            IBUS_TX_PIN,
            IBUS_RX_PIN,
            UART_PIN_NO_CHANGE,
            UART_PIN_NO_CHANGE));

    return ESP_OK;
}

esp_err_t receiver_update(void)
{
    int len = uart_read_bytes(
                    IBUS_UART,
                    ibus_packet,
                    IBUS_PACKET_SIZE,
                    pdMS_TO_TICKS(5));

    if(len != IBUS_PACKET_SIZE)
        return ESP_FAIL;

    if(ibus_packet[0] != 0x20)
        return ESP_FAIL;

    if(ibus_packet[1] != 0x40)
        return ESP_FAIL;

    if(!ibus_checksum(ibus_packet))
        return ESP_FAIL;

    rx.ch1 = ibus_packet[2]  | (ibus_packet[3]  << 8);
    rx.ch2 = ibus_packet[4]  | (ibus_packet[5]  << 8);
    rx.ch3 = ibus_packet[6]  | (ibus_packet[7]  << 8);
    rx.ch4 = ibus_packet[8]  | (ibus_packet[9]  << 8);
    rx.ch5 = ibus_packet[10] | (ibus_packet[11] << 8);
    rx.ch6 = ibus_packet[12] | (ibus_packet[13] << 8);

    return ESP_OK;
}

uint16_t receiver_get_roll(void)
{
    return rx.ch1;
}

uint16_t receiver_get_pitch(void)
{
    return rx.ch2;
}

uint16_t receiver_get_throttle(void)
{
    return rx.ch3;
}

uint16_t receiver_get_yaw(void)
{
    return rx.ch4;
}

uint16_t receiver_get_aux1(void)
{
    return rx.ch5;
}

uint16_t receiver_get_aux2(void)
{
    return rx.ch6;
}