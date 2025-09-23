/*
Header to define button module logic
*/

#include "btn.h"

/* ----------------------------------------------------------------------------
                                    CONSTANTS
---------------------------------------------------------------------------- */
#define BTN_DEBOUNCE_MS   20

#define BTN0_NODE   DT_ALIAS(sw0)
#define BTN1_NODE   DT_ALIAS(sw1)
#define BTN2_NODE   DT_ALIAS(sw2)
#define BTN3_NODE   DT_ALIAS(sw3)

/* ----------------------------------------------------------------------------
                                    TYPES
---------------------------------------------------------------------------- */
typedef struct btn_gpio_t {
  struct gpio_dt_spec spec; 
  char *name;
  bool pressed;
  struct gpio_callback cb;
  uint64_t last_event;
} btn_gpio;

/* ----------------------------------------------------------------------------
                            Private Function Prototypes
---------------------------------------------------------------------------- */
static int _config_btn(btn_gpio *btn);

static void _btn_interrupt_service_routine(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

/* ----------------------------------------------------------------------------
                                Global States
---------------------------------------------------------------------------- */
static btn_gpio _btn0 = {.spec=GPIO_DT_SPEC_GET(BTN0_NODE, gpios), .name="BTN0", .pressed=false, .last_event=0};
static btn_gpio _btn1 = {.spec=GPIO_DT_SPEC_GET(BTN1_NODE, gpios), .name="BTN1", .pressed=false, .last_event=0};
static btn_gpio _btn2 = {.spec=GPIO_DT_SPEC_GET(BTN2_NODE, gpios), .name="BTN2", .pressed=false, .last_event=0};
static btn_gpio _btn3 = {.spec=GPIO_DT_SPEC_GET(BTN3_NODE, gpios), .name="BTN3", .pressed=false, .last_event=0};
static btn_gpio *_btns[NUM_BTNS];

/* ----------------------------------------------------------------------------
                              Private Functions
---------------------------------------------------------------------------- */
/**
 * @brief Configures a gpio spec as a button
 * 
 * @param [in] btn the btn_gpio object to configure
 * 
 * @return Error code, < 0 on failures
 */
static int _config_btn(btn_gpio *btn) {
  if (!gpio_is_ready_dt(&btn->spec)) {
    ERROR("%s was not ready during init.", btn->name);
		return -EIO;
	} else if (0 > gpio_pin_configure_dt(&btn->spec, GPIO_INPUT)) {
    ERROR("%s could not be configured during init.", btn->name);
		return -EIO;
  } else if (0 > gpio_pin_interrupt_configure_dt(&btn->spec, GPIO_INT_EDGE_TO_ACTIVE)) {
    ERROR("%s interrupt could not be configured during init.", btn->name);
		return -EIO;
  } else {
    gpio_init_callback(&btn->cb, _btn_interrupt_service_routine, BIT(btn->spec.pin));
    gpio_add_callback(btn->spec.port, &btn->cb);
    DEBUG("Successfully configured %s", btn->name);
    return 0;
  }
}

/**
 * @brief Invoked as an interrupt when a button goes to the active state (high)
 * 
 * @param [in] dev The GPIO port that triggered the interrupt
 * @param [in] cb A pointer to the registered callback structure for this ISR
 * @param [in] pins A bitmask for all the GPIO pins that triggered this interrupt
 */
static void _btn_interrupt_service_routine(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
  // TODO: Handle getting / releasing mutex
  // TODO: Handle button debouncing using: k_uptime_get() to check time between current ISR and last ISR compared to BTN_DEBOUNCE_MS
}

/* ----------------------------------------------------------------------------
                              Public Functions
---------------------------------------------------------------------------- */
/**
 * @brief Inits all buttons
 * 
 * @return Error code, < 0 on failures
 */
int BTN_init() {
  _btns[0] = &_btn0;
  _btns[1] = &_btn1;
  _btns[2] = &_btn2;
  _btns[3] = &_btn3;

  for (uint8_t i = 0; i < NUM_BTNS; i++) {
    int rv = _config_btn(_btns[i]);
    if (rv < 0) {
      ERROR("Failed to configure %s", _btns[i]->name);
      return rv;
    }
  }

  return 0;
}

/**
 * @brief Checks if the given button is currently being pressed
 * 
 * @param [in] btn Which button to check
 * 
 * @return true if btn is being pressed
 */
bool BTN_is_pressed(btn_id btn) {
  if (btn >= NUM_BTNS || btn < 0) {
    ERROR("Couldn't check the state of an invalid button");
    return false;
  } else if (0 < gpio_pin_get_dt(&_btns[btn]->spec)) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief Checks if the given button has been pressed, clears internal state flag.
 *        Equivalent to calling BTN_check_pressed(BTNx) then calling BTN_clear_pressed(BTNx)
 * 
 * @param [in] btn Which button to check
 * 
 * @return true if btn has been pressed
 */
bool BTN_was_pressed(btn_id btn) {
  // TODO: Handle getting / releasing mutex
  return false;
}

/**
 * @brief Checks if the given button has been pressed, doesn't clear the internal state flag
 * 
 * @param [in] btn Which button to check
 * 
 * @return true if btn has been pressed
 */
bool BTN_check_pressed(btn_id btn) {
  if (btn >= NUM_BTNS || btn < 0) {
    ERROR("Couldn't check the state of an invalid button");
    return false;
  } else {
    return _btns[btn]->pressed;
  }
}

/**
 * @brief Clears the internal state flag of a given button
 * 
 * @param [in] btn Which button to clear
 */
void BTN_clear_pressed(btn_id btn) {
  // TODO: Handle getting / releasing mutex
}
