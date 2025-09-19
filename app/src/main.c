/*
Program entry point
*/

#include "includes.h"
#include "threads.h"
#include "init.h"
#include "led.c"

#define SLEEP_TIME_MS   250

int main(void)
{
	if (init_leds()){
    return 0;
  }

  if (toggle_led(LED0) || toggle_led(LED3)) {
    return 0;
  }

	while (1) {
    k_msleep(SLEEP_TIME_MS);

		if (toggle_led(LED0) || toggle_led(LED1) || toggle_led(LED2) || toggle_led(LED3)) {
			return 0;
		}
	}
	return 0;
}