#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app LANGUAGES C)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE modules/LED/led.c)
target_sources(app PRIVATE modules/BTN/btn.c)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
)
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/BTN
)
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/LED
)
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/DBG
)
