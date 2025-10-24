/*
 * main.c
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include <LED.h>
#include <BTN.h>

#define SLEEP_TIME_MS 1

// #define SW0_NODE DT_ALIAS(sw0)
// static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

// static struct gpio_callback button_isr_data;

// void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins){
//   printk("Button 1 pressed!\n");
// }

int main(void) {
  // int ret0;
  //   if (!gpio_is_ready_dt(&button0)){
  //     return 0;
  //   }

  //   ret0 = gpio_pin_configure_dt(&button0, GPIO_INPUT);
  //   if (0 > ret0){
  //     return 0;
  //   }

  //   ret0 = gpio_pin_configure_dt(&button0, GPIO_INPUT);
  //   if (0 > ret0){
  //     return 0;
  //   }

  //   ret0 = gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE);
  //   if (0 > ret0){
  //     return 0;
  //   }

  //   gpio_init_callback(&button_isr_data, button_isr, BIT(button0.pin));
  //   gpio_add_callback(button0.port, &button_isr_data);
  if (0> BTN_init()){
    return 0;
  }
  if (0 > LED_init()){
    return 0;
  }
  // bool pass_check;
  while(1) {
    // ret1 = gpio_pin_get_dt(&button0);
    // if (0 < ret1){
    //   printk("Button 1 pressed!\n");
    // }
    // int counter = 0;
    
    if (BTN_check_clear_pressed(BTN0)){
      LED_toggle(LED0);
      // LED_set(LED0, LED_ON);
      printk("Button 0 pressed!\n");
    }
    k_msleep(SLEEP_TIME_MS);
  }
	return 0;
}
