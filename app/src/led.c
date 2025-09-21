/*
Header to define led encapsulation logic
*/

#include "led.h"

/* ----------------------------------------------------------------------------
                                    CONSTANTS
---------------------------------------------------------------------------- */
#define BLINK_LED_STACK_SIZE    4096
#define BLINK_LED_PRIORITY      1

#define LED0_NODE   DT_ALIAS(led0)
#define LED1_NODE   DT_ALIAS(led1)
#define LED2_NODE   DT_ALIAS(led2)
#define LED3_NODE   DT_ALIAS(led3)

/* ----------------------------------------------------------------------------
                                    TYPES
---------------------------------------------------------------------------- */
typedef struct led_gpio_t {
  struct gpio_dt_spec spec; 
  char *name;
  bool is_blinking;
  led_frequency frequency;
  struct k_thread thread;
  k_tid_t tid;
} led_gpio;

/* ----------------------------------------------------------------------------
                            Private Function Prototypes
---------------------------------------------------------------------------- */
static int _config_led(const led_gpio *led);

void _suspend_blink_thread(led_inst led_instance);

void _blink_led_loop(void *led_instance, void *p2, void *p3);

/* ----------------------------------------------------------------------------
                                Global States
---------------------------------------------------------------------------- */
static led_gpio _led_0 = {.spec=GPIO_DT_SPEC_GET(LED0_NODE, gpios), .name="LED0", .is_blinking=false};
static led_gpio _led_1 = {.spec=GPIO_DT_SPEC_GET(LED1_NODE, gpios), .name="LED1", .is_blinking=false};
static led_gpio _led_2 = {.spec=GPIO_DT_SPEC_GET(LED2_NODE, gpios), .name="LED2", .is_blinking=false};
static led_gpio _led_3 = {.spec=GPIO_DT_SPEC_GET(LED3_NODE, gpios), .name="LED3", .is_blinking=false};
static led_gpio *_leds[NUM_LEDS];

K_THREAD_STACK_DEFINE(_blink_led0_stack, BLINK_LED_STACK_SIZE);
K_THREAD_STACK_DEFINE(_blink_led1_stack, BLINK_LED_STACK_SIZE);
K_THREAD_STACK_DEFINE(_blink_led2_stack, BLINK_LED_STACK_SIZE);
K_THREAD_STACK_DEFINE(_blink_led3_stack, BLINK_LED_STACK_SIZE);

/* ----------------------------------------------------------------------------
                              Private Functions
---------------------------------------------------------------------------- */
/**
 * @brief Configures a gpio spec as an led, returns 1 on failures
 * 
 * @param [in] led the led_gpio object to configure
 * 
 * @return Error code, < 0 on failures
 */
static int _config_led(const led_gpio *led) {
  if (!gpio_is_ready_dt(&led->spec)) {
    ERROR("%s was not ready during init.", led->name);
		return -EIO;
	} else if (0 > gpio_pin_configure_dt(&led->spec, GPIO_OUTPUT_INACTIVE)) {
    ERROR("%s could not be configured during init.", led->name);
		return -EIO;
  } else {
    DEBUG("Successfully configured %s", led->name);
    return 0;
  }
}

/**
 * @brief Acts as a generic blink led thread
 * 
 * @param [in] led_instance The led instance to blink
 * @param [in] p2 Unused thread parameter 2
 * @param [in] p2 Unused thread parameter 3
 */
void _blink_led_loop(void *led_instance, void *p2, void *p3) {
  led_inst _led_instance = (led_inst)(uintptr_t)led_instance;
  int _frequency = (int)_leds[_led_instance]->frequency;

  while (1) {
    LED_toggle(_led_instance);
    k_msleep(500 / _frequency);
  }
}

/* ----------------------------------------------------------------------------
                              Public Functions
---------------------------------------------------------------------------- */
/**
 * @brief Inits all LEDs
 * 
 * @return Error code, < 0 on failures
 */
int LED_init() {
  _leds[0] = &_led_0;
  _leds[1] = &_led_1;
  _leds[2] = &_led_2;
  _leds[3] = &_led_3;

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    int rv = _config_led(_leds[i]);
    if (rv < 0) {
      ERROR("Failed to configure %s", _leds[i]->name);
      return rv;
    }
  }

  _led_0.tid = k_thread_create(
    &_led_0.thread,
    _blink_led0_stack,
    K_THREAD_STACK_SIZEOF(_blink_led0_stack),
    _blink_led_loop,
    (void *)(uintptr_t)LED0, NULL, NULL,
    BLINK_LED_PRIORITY,
    0,
    K_NO_WAIT
  );
  k_thread_suspend(_led_0.tid);
  DEBUG("Initialized blink thread for LED0");

  _led_1.tid = k_thread_create(
    &_led_1.thread,
    _blink_led1_stack,
    K_THREAD_STACK_SIZEOF(_blink_led1_stack),
    _blink_led_loop,
    (void *)(uintptr_t)LED1, NULL, NULL,
    BLINK_LED_PRIORITY,
    0,
    K_NO_WAIT
  );
  k_thread_suspend(_led_1.tid);
  DEBUG("Initialized blink thread for LED1");

  _led_2.tid = k_thread_create(
    &_led_2.thread,
    _blink_led2_stack,
    K_THREAD_STACK_SIZEOF(_blink_led2_stack),
    _blink_led_loop,
    (void *)(uintptr_t)LED2, NULL, NULL,
    BLINK_LED_PRIORITY,
    0,
    K_NO_WAIT
  );
  k_thread_suspend(_led_2.tid);
  DEBUG("Initialized blink thread for LED2");

  _led_3.tid = k_thread_create(
    &_led_3.thread,
    _blink_led3_stack,
    K_THREAD_STACK_SIZEOF(_blink_led3_stack),
    _blink_led_loop,
    (void *)(uintptr_t)LED3, NULL, NULL,
    BLINK_LED_PRIORITY,
    0,
    K_NO_WAIT
  );
  k_thread_suspend(_led_3.tid);
  DEBUG("Initialized blink thread for LED3");

  return 0;
}

/**
 * @brief Toggle specified led
 * 
 * @param [in] led_instance The led instance to toggle
 * 
 * @return Error code, < 0 on failures
 */
int LED_toggle(led_inst led_instance) {
  if (led_instance >= NUM_LEDS || led_instance < 0) {
    ERROR("Couldn't toggle invalid LED: LED%d", led_instance);
    return -EINVAL;
  } else {
    int rv = gpio_pin_toggle_dt(&_leds[led_instance]->spec);
    if (rv) {
      ERROR("Failed to toggle %s", _leds[led_instance]->name);
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
 * @return Error code, < 0 on failures
 */
int LED_set(led_inst led_instance, led_state new_state) {
  if (led_instance >= NUM_LEDS || led_instance < 0) {
    ERROR("Couldn't set invalid LED: LED%d", led_instance);
    return -EINVAL;
  }
  
  if (_leds[led_instance]->is_blinking){
    _leds[led_instance]->is_blinking = false;
    k_thread_suspend(_leds[led_instance]->tid);
    DEBUG("Successfully stopped %s blink thread", _leds[led_instance]->name);
  }

  int rv = gpio_pin_set_dt(&_leds[led_instance]->spec, new_state);
  if (rv) {
    ERROR("Failed to set %s", _leds[led_instance]->name);
  }
  return rv;
}

/**
 * @brief Blinks the given LED at the given frequency
 * 
 * @param [in] led_instance The led instance to blink
 * @param [in] frequency The frequency to blink the led at
 */
void LED_blink(led_inst led_instance, led_frequency frequency) {
  _leds[led_instance]->frequency = frequency;
  if (!_leds[led_instance]->is_blinking) {
    _leds[led_instance]->is_blinking = true;
    k_thread_resume(_leds[led_instance]->tid);
    DEBUG("Successfully started %s blink thread", _leds[led_instance]->name);
  }
}
