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
  LED0 = 0,
  LED1,
  LED2,
  LED3,
  NUM_LEDS,
} led_inst;

typedef enum led_state_t {
  LED_OFF = 0,
  LED_ON,
} led_state;

typedef enum led_frequency_t {
  LED_1HZ = 1,
  LED_2HZ = 2,
  LED_4HZ = 4,
  LED_5HZ = 5,
  LED_8HZ = 8,
  LED_10HZ = 10,
  LED_16HZ = 16,
} led_frequency;

/* ----------------------------------------------------------------------------
                              Public Functions
---------------------------------------------------------------------------- */
int LED_init();

int LED_toggle(led_inst led_instance);

int LED_set(led_inst led_instance, led_state new_state);

void LED_blink(led_inst led_instance, led_frequency frequency);

#endif