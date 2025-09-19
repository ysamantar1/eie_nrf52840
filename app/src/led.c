/*
Header to define led encapsulation logic
*/

#include "led.h"

/* ----------------------------------------------------------------------------
                                    CONSTANTS
---------------------------------------------------------------------------- */
#define LED0_NODE   DT_ALIAS(led0)
#define LED1_NODE   DT_ALIAS(led1)
#define LED2_NODE   DT_ALIAS(led2)
#define LED3_NODE   DT_ALIAS(led3)

/* ----------------------------------------------------------------------------
                                    TYPES
---------------------------------------------------------------------------- */
typedef struct led_gpio_t {
  const gpio_dt_spec_t spec; 
  const char *name;
} led_gpio;

/* ----------------------------------------------------------------------------
                                Global States
---------------------------------------------------------------------------- */
static const led_gpio _led_0 = {.spec=GPIO_DT_SPEC_GET(LED0_NODE, gpios), .name="LED0"};
static const led_gpio _led_1 = {.spec=GPIO_DT_SPEC_GET(LED1_NODE, gpios), .name="LED1"};
static const led_gpio _led_2 = {.spec=GPIO_DT_SPEC_GET(LED2_NODE, gpios), .name="LED2"};
static const led_gpio _led_3 = {.spec=GPIO_DT_SPEC_GET(LED3_NODE, gpios), .name="LED3"};
static const led_gpio _leds[NUM_LEDS] = {_led_0, _led_1, _led_2, _led_3};

/* ----------------------------------------------------------------------------
                              Private Functions
---------------------------------------------------------------------------- */
/**
 * @brief Configures a gpio spec as an led, returns 1 on failures
 * 
 * @param [in] led the led_gpio object to configure
 * 
 * @return Error code, > 0 on failures
 */
static int _config_led(const led_gpio *led) {
  if (!gpio_is_ready_dt(&led->spec)) {
    DEBUG("%s was not ready during init.", led->name);
		return -EIO;
	} else if (0 > gpio_pin_configure_dt(&led->spec, GPIO_OUTPUT_INACTIVE)) {
    DEBUG("%s could not be configured during init.", led->name);
		return -EIO;
  } else {
    return 0;
  }
}

/* ----------------------------------------------------------------------------
                              Public Functions
---------------------------------------------------------------------------- */
/**
 * @brief Inits all LEDs
 * 
 * @return Error code, > 0 on failures
 */
int init_leds() {
  int rv = 0;
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    rv = _config_led(&_leds[i]);
    if (rv < 0) {
      DEBUG("Failed to configure %s\n", _leds[i].name);
      return rv;
    }
  }
  return rv;
}

/**
 * @brief Toggle specified led
 * 
 * @param [in] led_instance The led instance to toggle
 * 
 * @return Error code, > 0 on failures
 */
int toggle_led(led_inst led_instance) {
  if (led_instance >= NUM_LEDS || led_instance < 0) {
    DEBUG("Couldn't toggle invalid LED: LED%d\n", led_instance);
    return -EINVAL;
  } else {
    int rv = gpio_pin_toggle_dt(&_leds[led_instance].spec);
    if (rv) {
      DEBUG("Failed to toggle %s\n", _leds[led_instance].name);
    }
    return rv;
  }
}

/**
 * @brief Set specified led to given state
 * 
 * @param [in] led_instance The led instance to set
 * @param [in] new_state The state to set the led to
 * 
 * @return Error code, > 0 on failures
 */
int set_led(led_inst led_instance, led_state new_state) {
  if (led_instance >= NUM_LEDS || led_instance < 0) {
    DEBUG("Couldn't set invalid LED: LED%d\n", led_instance);
    return -EINVAL;
  } else {
    int rv = gpio_pin_set_dt(&_leds[led_instance].spec, new_state);
    if (rv) {
      DEBUG("Failed to set %s\n", _leds[led_instance].name);
    }
    return rv;
  }
}