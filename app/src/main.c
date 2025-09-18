/*
Program entry point
*/

#include "includes.h"
#include "threads.h"
#include "init.h"
#include "led.c"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   250

int main(void)
{
	if (init_leds()){
    DEBUG("Failed to init LEDS");
    return 0;
  }

	while (1) {
		if (toggle_led(LED0)) {
      DEBUG("Failed to toggle LED");
			return 0;
		}
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}