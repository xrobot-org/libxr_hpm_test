/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app_main.h"
#include "board.h"

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_gpio_pins();
    app_main();
    return 0;
}
