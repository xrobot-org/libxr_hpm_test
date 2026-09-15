# Copyright (c) 2026
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_NAME hpm5361evklite_gpio CACHE STRING "Application name")
set(BOARD hpm5361evklite CACHE STRING "HPM board name")
set(BOARD_SEARCH_PATH ${CMAKE_CURRENT_LIST_DIR}/boards CACHE PATH "Custom HPM board search path")
set(CONFIG_DMA_MGR 1)
# UART0 pins are assigned to MCAN0. UART3 is owned by LibXR after app startup.
set(CONFIG_NDEBUG_CONSOLE 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(hpm5361evklite_gpio C CXX ASM)
sdk_compile_definitions(-DCONFIG_NDEBUG_CONSOLE=1 -DBOARD_SHOW_CLOCK=0 -DBOARD_SHOW_BANNER=0)

sdk_app_inc(User)
sdk_app_src(src/main.c)
sdk_app_src(User/app_main.cpp)

generate_ide_projects()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/LibXR.CMake)
