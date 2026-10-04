# Copyright (c) 2026
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_NAME hpm5361evklite_gpio CACHE STRING "Application name")
set(BOARD hpm5361evklite CACHE STRING "HPM board name")
set(BOARD_SEARCH_PATH ${CMAKE_CURRENT_LIST_DIR}/boards CACHE PATH "Custom HPM board search path")
set(CONFIG_DMA_MGR 1)

set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_LIST_DIR}/cmake/compiler_probe.cmake")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(hpm5361evklite_gpio C CXX ASM)
# The SDK console runs on UART3 (PB15 TX / PB14 RX) instead of the board default UART0.
sdk_compile_definitions(
    -DBOARD_CONSOLE_UART_BASE=HPM_UART3
    -DBOARD_CONSOLE_UART_CLK_NAME=clock_uart3
    -DBOARD_CONSOLE_UART_IRQ=IRQn_UART3
    -DBOARD_CONSOLE_UART_TX_DMA_REQ=HPM_DMA_SRC_UART3_TX
    -DBOARD_CONSOLE_UART_RX_DMA_REQ=HPM_DMA_SRC_UART3_RX
    -DBOARD_SHOW_CLOCK=0
    -DBOARD_SHOW_BANNER=0)

sdk_app_inc(User)
sdk_app_src(src/main.c)
sdk_app_src(User/app_main.cpp)

generate_ide_projects()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/LibXR.CMake)
