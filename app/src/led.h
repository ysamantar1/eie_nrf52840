/*
Header to define led interface
*/

#ifndef LED_H
#define LED_H

#include "includes.h"

/* ----------------------------------------------------------------------------
                                    TYPES
---------------------------------------------------------------------------- */
typedef enum led_inst_t {
  LED0,
  LED1,
  LED2,
  LED3,
  NUM_LEDS,
} led_inst;

/* ----------------------------------------------------------------------------
                              Public Functions
---------------------------------------------------------------------------- */
int init_leds();

int toggle_led(led_inst led_instance);

#endif