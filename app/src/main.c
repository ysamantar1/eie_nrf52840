/*
Program entry point
*/

#include "includes.h"
#include "LED.h"
#include "BTN.h"

int main(void)
{
	if (LED_init()){
    return 0;
  } else if (BTN_init()) {
    return 0;
  }

  while(1) {
    k_msleep(1);
    if (BTN_was_pressed(BTN0)) {
      LED_toggle(LED0);
    }
  }
	return 0;
}