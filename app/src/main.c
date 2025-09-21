/*
Program entry point
*/

#include "includes.h"
#include "led.h"

int main(void)
{
	if (LED_init()){
    return 0;
  }

  LED_blink(LED0, LED_5HZ);

  while(1) {
    k_msleep(1000);
  }
	return 0;
}