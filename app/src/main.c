/**
 * @file main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include <lvgl.h>

#include "BTN.h"
#include "LED.h"
// #include "lv data obj.h"

#define SLEEP_MS 1

static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static lv_obj_t *screen = NULL;

int main(void) {
  if(!device_is_ready(display_dev)) {
    return 0;
  }
  screen = lv_screen_active();
  if(screen == NULL){
    return 0;
  }

  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  flashes Hello World
  lv_obj_t *label = lv_label_create(screen);
  lv_label_set_text(label, "Hello World!");

  for (uint8_t i=0; i < NUM_LEDS; i++){
    lv_obj_t *ui_btn = lv_label_create(screen);
    // place the buttons in a 2x2 grid in the center of the screen
    // matching the orientations of the LEDs on the board
    lv_obj_align(ui_btn, LV_ALIGN_CENTER, 50 * (i % 2 ? 1 : -1), 20 * (i < 2 ? -1 : 1));
    lv_obj_t *button_label = lv_label_create(ui_btn);
    char label_text[10];
    snprintf(label_text, 10, " %d", i);
    lv_label_set_text(button_label, label_text);
    lv_obj_align(button_label, LV_ALIGN_CENTER, 0, 0);
  }

  display_blanking_off(display_dev);
  while (1) {
    lv_timer_handler();
    k_msleep(SLEEP_MS);
  }
  return 0;
}
