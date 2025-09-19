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

typedef enum led_state_t {
  LOW,
  HIGH,
  NUM_STATES,
} led_state;

/* ----------------------------------------------------------------------------
                              Public Functions
---------------------------------------------------------------------------- */
int init_leds();

int toggle_led(led_inst led_instance);

int set_led(led_inst led_instance, led_state new_state);

#endif