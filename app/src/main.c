/*
Program entry point
*/

#include "includes.h"
#include "led.h"
#include "btn.h"

int main(void)
{
	if (LED_init()){
    return 0;
  } else if (BTN_init()) {
    return 0;
  }

  while(1) {
    k_msleep(100);
    if (BTN_is_pressed(BTN0)) {
      LED_toggle(LED0);
    }
  }
	return 0;
}