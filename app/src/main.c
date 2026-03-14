/**
 * @file main.c
 */

#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/printk.h>

#include <zephyr/device.h>
#include <lvgl.h>
#include <zephyr/drivers/display.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "BTN.h"
#include "LED.h"

/* --------------------------------------------------------------------------
 * UI State Machine
 * -------------------------------------------------------------------------- */
typedef enum {
    UI_STATE_ADVERTISING,
    UI_STATE_CONNECTED,
    UI_STATE_PASSKEY,
    UI_STATE_PAIRED,
    UI_STATE_PAIR_FAILED,
} ui_state_t;

static K_MUTEX_DEFINE(ui_mutex);
static ui_state_t  ui_state        = UI_STATE_ADVERTISING;
static char        ui_passkey[16]  = {0};
static bool        ui_needs_update = true;
 
/* LVGL objects — created once in ui_init(), updated in ui_render() */
static lv_obj_t *bg_rect     = NULL;
static lv_obj_t *label_title = NULL;
static lv_obj_t *label_sub   = NULL;
 

static void ui_set_state(ui_state_t state, const char *passkey_str)
{
    k_mutex_lock(&ui_mutex, K_FOREVER);
    ui_state = state;
    if (passkey_str) {
        strncpy(ui_passkey, passkey_str, sizeof(ui_passkey) - 1);
        ui_passkey[sizeof(ui_passkey) - 1] = '\0';
    }
    ui_needs_update = true;
    k_mutex_unlock(&ui_mutex);
}


static void ui_render(void)
{
    k_mutex_lock(&ui_mutex, K_FOREVER);
 
    if (!ui_needs_update) {
        k_mutex_unlock(&ui_mutex);
        return;
    }
 
    ui_state_t state = ui_state;
    char       pk[16];
 
    strncpy(pk, ui_passkey, sizeof(pk));
    ui_needs_update = false;
    k_mutex_unlock(&ui_mutex);
 
    /* Colours and text for each state */
    lv_color_t col_bg, col_title, col_sub;
    const char *title_text;
    const char *sub_text;
    const lv_font_t *title_font;
 
    switch (state) {
 
    case UI_STATE_ADVERTISING:
        col_bg     = lv_color_hex(0x003080);
        col_title  = lv_color_hex(0xFFFFFF);
        col_sub    = lv_color_hex(0xADD8E6);
        title_text = "BLE Secure Demo";
        sub_text   = "Open nRF Connect\non your phone\nand connect.";
        title_font = &lv_font_montserrat_48;
        break;
 
    case UI_STATE_CONNECTED:
        col_bg     = lv_color_hex(0x806000);
        col_title  = lv_color_hex(0xFFFF00);
        col_sub    = lv_color_hex(0xFFFFFF);
        title_text = "Connected!";
        sub_text   = "Waiting for\npairing request...";
        title_font = &lv_font_montserrat_28;
        break;
 
    case UI_STATE_PASSKEY:
        col_bg     = lv_color_hex(0x1A1A2E);
        col_title  = lv_color_hex(0xFFFFFF);
        col_sub    = lv_color_hex(0xFFD700);
        title_text = pk;               /* "123456" */
        sub_text   = "Match this passkey\non your phone\n(nRF Connect)";
        title_font = &lv_font_montserrat_48; /* big digits */
        break;
 
    case UI_STATE_PAIRED:
        col_bg     = lv_color_hex(0x004000);
        col_title  = lv_color_hex(0x00FF80);
        col_sub    = lv_color_hex(0xFFFFFF);
        title_text = "Paired!";
        sub_text   = "Secure link active."; //\nGATT service\nnow accessible.";
        title_font = &lv_font_montserrat_48;
        break;
 
    case UI_STATE_PAIR_FAILED:
    default:
        col_bg     = lv_color_hex(0x600000);
        col_title  = lv_color_hex(0xFF4040);
        col_sub    = lv_color_hex(0xFFFFFF);
        title_text = "Pairing FAILED";
        sub_text   = "Check phone and\nretry connection.";
        title_font = &lv_font_montserrat_28;
        break;
    }
 
    /* Apply background colour */
    lv_obj_set_style_bg_color(bg_rect, col_bg, LV_PART_MAIN);
 
    /* Apply title */
    lv_obj_set_style_text_font(label_title, title_font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_title, col_title, LV_PART_MAIN);
    lv_label_set_text(label_title, title_text);
 
    /* Apply subtitle */
    lv_obj_set_style_text_color(label_sub, col_sub, LV_PART_MAIN);
    lv_label_set_text(label_sub, sub_text);
 
    lv_refr_now(NULL);
}

/* --------------------------------------------------------------------------
 * This function initializes LVGL screen objects
 * -------------------------------------------------------------------------- */
static int ui_init(void)
{
    const struct device *display_dev =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
 
    if (!device_is_ready(display_dev)) {
        printk("[UI] Display device not ready\n");
        return -ENODEV;
    }
 
    display_blanking_off(display_dev);
 
    lv_obj_t *scr = lv_scr_act();
 
    /* Full-screen coloured background */
    bg_rect = lv_obj_create(scr);
    lv_obj_set_size(bg_rect, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(bg_rect, 0, 0);
    lv_obj_set_style_border_width(bg_rect, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(bg_rect, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bg_rect, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bg_rect, lv_color_hex(0x003080), LV_PART_MAIN);
 
    /* Title label*/
    label_title = lv_label_create(bg_rect);
    lv_label_set_long_mode(label_title, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label_title, LV_HOR_RES - 20);
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_align(label_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(label_title, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_title, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label_title, "Initialising...");
 
    /* Sub-title label*/
    label_sub = lv_label_create(bg_rect);
    lv_label_set_long_mode(label_sub, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label_sub, LV_HOR_RES - 20);
    lv_obj_align(label_sub, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_text_align(label_sub, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(label_sub, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_sub, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label_sub, "");
 
    printk("[UI] Display initialised (%d x %d)\n", LV_HOR_RES, LV_VER_RES);
    return 0;
}

/* --------------------------------------------------------------------------
 * GATT Callbacks
 * -------------------------------------------------------------------------- */
static const char secure_data[] = "SECRET: Pairing Successful! Secure BLE Demo.";
static uint8_t    write_buf[64];
static uint16_t   write_buf_len;
 
static ssize_t read_secure_data(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                void *buf, uint16_t len, uint16_t offset)
{
    const char *value = attr->user_data;
 
    printk("[GATT] Secure read from authenticated peer.\n");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
}
 
static ssize_t write_secure_data(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 const void *buf, uint16_t len,
                                 uint16_t offset, uint8_t flags)
{
    if (offset + len > sizeof(write_buf) - 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
 
    memcpy(write_buf + offset, buf, len);
    write_buf_len = offset + len;
    write_buf[write_buf_len] = '\0';
    printk("[GATT] Secure write (%u bytes): %s\n", write_buf_len, write_buf);
    return len;
}

/* --------------------------------------------------------------------------
 * Custom UUIDs
 * -------------------------------------------------------------------------- */
#define BT_UUID_SECURE_DEMO_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)
#define BT_UUID_SECURE_READ_CHAR_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1)
#define BT_UUID_SECURE_WRITE_CHAR_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2)
 
#define BT_UUID_SECURE_DEMO_SERVICE  BT_UUID_DECLARE_128(BT_UUID_SECURE_DEMO_SERVICE_VAL)
#define BT_UUID_SECURE_READ_CHAR     BT_UUID_DECLARE_128(BT_UUID_SECURE_READ_CHAR_VAL)
#define BT_UUID_SECURE_WRITE_CHAR    BT_UUID_DECLARE_128(BT_UUID_SECURE_WRITE_CHAR_VAL)
 
BT_GATT_SERVICE_DEFINE(secure_demo_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_SECURE_DEMO_SERVICE),
    BT_GATT_CHARACTERISTIC(BT_UUID_SECURE_READ_CHAR,
                           BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ_AUTHEN,
                           read_secure_data, NULL,
                           (void *)secure_data),
    BT_GATT_CHARACTERISTIC(BT_UUID_SECURE_WRITE_CHAR,
                           BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_WRITE_AUTHEN,
                           NULL, write_secure_data, NULL),
);
 
/* --------------------------------------------------------------------------
 * Advertising data
 * -------------------------------------------------------------------------- */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_SHORTENED, "Secure Demo", 10),
};
 
static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, "Secure BLE Demo", 19),
};
 
static struct bt_conn *current_conn;

/* --------------------------------------------------------------------------
 * Auth / Pairing Callbacks
 * -------------------------------------------------------------------------- */
/* This function displays passkey and requires user to input the passkey displayed */
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];
    char pk_str[16];
 
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    snprintk(pk_str, sizeof(pk_str), "%06u", passkey);
 
    printk("\n============================================\n");
    printk("  PASSKEY REQUIRED\n");
    printk("  Peer:    %s\n", addr);
    printk("  Passkey: %s\n", pk_str);
    printk("  Write code on nRF Connect mobile app\n");
    printk("============================================\n\n");
 
    ui_set_state(UI_STATE_PASSKEY, pk_str);
}

/* This function confirms whether the passkey matches the user (Uses LSE and NC and is L4)*/
static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];
    char pk_str[16];
 
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    snprintk(pk_str, sizeof(pk_str), "%06u", passkey);
 
    printk("\n============================================\n");
    printk("  NUMERIC COMPARISON\n");
    printk("  Passkey: %s — confirm on your phone!\n", pk_str);
    printk("  (Auto-confirming on device side)\n");
    printk("============================================\n\n");
 
    ui_set_state(UI_STATE_PASSKEY, pk_str);
    bt_conn_auth_passkey_confirm(conn);
}

/* This function addresses the case where pairing fails (wrong passkey / unconfirmed NC) */
static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("[AUTH] Pairing cancelled by %s\n", addr);
    ui_set_state(UI_STATE_PAIR_FAILED, NULL);
}

/* This function addresses the case where the pairing is successful */ 
static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("\n[AUTH] Pairing complete — %s (bonded: %s)\n\n",
           addr, bonded ? "YES" : "NO");
    ui_set_state(UI_STATE_PAIRED, NULL);
}

/* This function addresses the case where the pairing fails */
static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("[AUTH] Pairing FAILED — %s (reason %d)\n", addr, reason);
    ui_set_state(UI_STATE_PAIR_FAILED, NULL);
}
 
static struct bt_conn_auth_cb auth_cb = {
    .passkey_display = auth_passkey_display,
    .passkey_confirm = auth_passkey_confirm,
    .cancel          = auth_cancel,
};
 
static struct bt_conn_auth_info_cb auth_info_cb = {
    .pairing_complete = pairing_complete,
    .pairing_failed   = pairing_failed,
};
 
/* --------------------------------------------------------------------------
 * Connection Callbacks
 * -------------------------------------------------------------------------- */
static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
 
    if (err) {
        printk("[CONN] Connection failed (err %u)\n", err);
        return;
    }
 
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("\n[CONN] Connected: %s\n", addr);
 
    current_conn = bt_conn_ref(conn);
    ui_set_state(UI_STATE_CONNECTED, NULL);
 
    #ifdef CONFIG_BT_SMP
    int sec_err = bt_conn_set_security(conn, BT_SECURITY_L4);
    if (sec_err) {
        printk("[CONN] Failed to set security (err %d)\n", sec_err);
    }
    #endif // CONFIG_BT_SMP
    
}
 
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
 
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("\n[CONN] Disconnected: %s (reason 0x%02x)\n\n", addr, reason);
 
    if (current_conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }
 
    ui_set_state(UI_STATE_ADVERTISING, NULL);
 
    bt_unpair(BT_ID_DEFAULT, bt_conn_get_dst(conn)); // unpairs after disconnected (USED FOR DEMO ONLY)
    int err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
                               sd, ARRAY_SIZE(sd));

    if (err) {
        printk("[ADV] Failed to restart advertising (err %d)\n", err);
    }
}
 
static void security_changed(struct bt_conn *conn, bt_security_t level,
                              enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
 
    if (!err) {
        printk("[SEC] Security L%d active for %s\n", level, addr);
    } else {
        printk("[SEC] Security change FAILED for %s (err %d)\n", addr, err);
    }
}
 
BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected        = connected,
    .disconnected     = disconnected,
    // .security_changed = security_changed,
};

#define SLEEP_MS 1

int main(void) {
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  int err;
  /* Initialise display — non-fatal if absent */
  err = ui_init();
  if (err) {
    printk("[UI] No display found, continuing without LCD.\n");
  } else {
    ui_render(); /* show initial "advertising" screen */
  }
 
  /* Initialise Bluetooth */
  err = bt_enable(NULL);
  if (err) {
    printk("[BT] Bluetooth init failed (err %d)\n", err);
    return err;
  }
  settings_load();

  #ifdef CONFIG_BT_SMP
  err = bt_conn_auth_cb_register(&auth_cb);
  if (err) { return err; }
 
  err = bt_conn_auth_info_cb_register(&auth_info_cb);
  if (err) { return err; }
  #endif
 
  err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
                          sd, ARRAY_SIZE(sd));
  if (err) {
    printk("[ADV] Advertising start failed (err %d)\n", err);
    return err;
  }

  printk("[ADV] Advertising as \"BLE SecureDemo\".\n");
  printk("[ADV] Passkey will appear on LCD and serial console.\n\n");

  while (1) {
    uint32_t sleep_ms = lv_task_handler();
    ui_render();
    k_msleep(MIN(sleep_ms,10));
  }
  return 0;
}
