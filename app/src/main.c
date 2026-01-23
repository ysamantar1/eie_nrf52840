/*
 * main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 100
#define ADDR 0x38
#define TD_STATUS 0x02
#define P1_XH 0x03
#define P1_XL 0x04
#define P1_YH 0x05
#define P1_YL 0x06

#define TOUCH_EVENT_MASK 0xC0
#define TOUCH_EVENT_SHIFT 6

#define TOUCH_POS_MSB_MASK 0x0F

typedef enum {
  TOUCH_EVENT_PRESS_DOWN = 0b00u,
  TOUCH_EVENT_LIFT_UP = 0b01u,
  TOUCH_EVENT_CONTACT = 0b10u,
  TOUCH_EVENT_NO_EVENT = 0b11u
} touch_event_t;

#define ARDUINO_I2C_NODE DT_NODE_LABEL(arduino_i2c)
static const struct device* dev = DEVICE_DT_GET(ARDUINO_I2C_NODE);

// This helper function will be used to read touch events from the screen in our main loop
void touch_control_cmd_rsp(uint8_t cmd, uint_t * rsp){
  struct i2c_msg cmd_rsp[2] = {
    [0]={.buf=&cmd, .len=1, .flags=I2C_MSG_WRITE},
    [1]={.buf=&cmd, .len=1, .flags=I2C_MSG_RESTART | I2C_MSG_READ | I2C_MSG_STOP}
  };
  i2c_transfer(dev, msgs: cmd_rsp_msg, num_msgs:2, addr: ADDR);
}

int main(void) {
  ////////////////////////////////////////////////// SPI IMPLEMENTATION ///////////////////////////////////////////////////
  
  
  ////////////////////////////////////////////////// I2C IMPLEMENTATION ///////////////////////////////////////////////////

  if (!device_is_ready(dev)){
    return 0;
  }

  if (0 > i2c_configure(dev, dev_config: I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_CONTROLLER)){
    return 0;
  }

  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  while(1) {
    uint8_t touch status;
    touch_control_cmd_rsp(cmd: TD_STATUS, rsp: &touch_status);
    if(touch_status == 1) {
      
    }
    k_msleep(ms:SLEEP_MS);
  }
	return 0;
}
