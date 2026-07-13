/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include "hpm_uart_drv.h"

#define LED_TOGGLE_PERIOD_MS (500U)
#define GPIO_POLL_PERIOD_MS  (20U)

int main(void)
{
    uint32_t elapsed_ms = 0;
    uint32_t heartbeat_ms = 0;
    uint32_t heartbeat_count = 0;
    uint8_t last_button_state;

    board_init();
    board_init_led_pins();
    board_init_gpio_pins();

    last_button_state = gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    printf("HPM5361EVKLite GPIO demo\r\n");
    printf("board: %s\r\n", BOARD_NAME);
    printf("LED: PA10 active low, button: PA03 active high\r\n");

    while (1) {
        uint8_t button_state = gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
        uint8_t rx_byte;

        while (status_success == uart_try_receive_byte((UART_Type *)BOARD_CONSOLE_UART_BASE, &rx_byte)) {
            char printable = (rx_byte >= 0x20U && rx_byte <= 0x7eU) ? (char)rx_byte : '.';
            printf("rx 0x%02x '%c'\r\n", rx_byte, printable);
        }

        if (button_state != last_button_state) {
            printf("button: %s\r\n",
                   button_state == BOARD_BUTTON_PRESSED_VALUE ? "pressed" : "released");
            last_button_state = button_state;
        }

        if (elapsed_ms >= LED_TOGGLE_PERIOD_MS) {
            board_led_toggle();
            elapsed_ms = 0;
        }

        if (heartbeat_ms >= 1000U) {
            printf("heartbeat %lu, button=%u\r\n", heartbeat_count++, button_state);
            heartbeat_ms = 0;
        }

        board_delay_ms(GPIO_POLL_PERIOD_MS);
        elapsed_ms += GPIO_POLL_PERIOD_MS;
        heartbeat_ms += GPIO_POLL_PERIOD_MS;
    }
}
