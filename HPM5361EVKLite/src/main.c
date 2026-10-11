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
    /* I2C3 clock and pins for LibXR::HPMI2C in app_main(). */
    board_init_i2c_clock(BOARD_APP_I2C_BASE);
    init_i2c_pins(BOARD_APP_I2C_BASE);
    app_main();
    return 0;
}
