/*
Header to define constants and useful macros
*/

#ifndef DEFINES_H
#define DEFINESS_H

#include "includes.h"

// Configs
#define ERROR_LOGS  1
#define DEBUG_LOGS  0

// Logging
#if ERROR_LOGS
#define ERROR(fmt, ...) printk("Error: " fmt "\n", ##__VA_ARGS__)
#else
#define ERROR(...) do { } while (0)
#endif

#if DEBUG_LOGS
#define DEBUG(fmt, ...) printk("Debug: " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG(...) do { } while (0)
#endif

#define CONSOLE(fmt, ...) printk(fmt, ##__VA_ARGS__)

#endif