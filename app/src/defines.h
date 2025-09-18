/*
Header to define constants and useful macros
*/

#ifndef DEFINES_H
#define DEFINESS_H

#include "includes.h"

// Configs
#define DEBUG_LOGS 1

// Logging
#if DEBUG_LOGS
#define DEBUG(fmt, ...) printk("Debug: " fmt, ##__VA_ARGS__)
#else
#define DEBUG(...) do { } while (0)
#endif

// Types
typedef struct gpio_dt_spec gpio_dt_spec_t;

#endif