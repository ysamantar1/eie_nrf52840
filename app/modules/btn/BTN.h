/*
Header to define button interface
*/

#ifndef BTN_H
#define BTN_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

/* ----------------------------------------------------------------------------
                                    TYPES
---------------------------------------------------------------------------- */
typedef enum btn_id_t {
  BTN0 = 0,
  BTN1,
  BTN2,
  BTN3,
  NUM_BTNS,
} btn_id;

/* ----------------------------------------------------------------------------
                              Public Functions
---------------------------------------------------------------------------- */
int BTN_init();

bool BTN_is_pressed(btn_id btn);

bool BTN_was_pressed(btn_id btn);

bool BTN_check_pressed(btn_id btn);

void BTN_clear_pressed(btn_id btn);

#endif