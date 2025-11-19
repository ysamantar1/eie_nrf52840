/*
 * main.c
 */

/* IMPORTS -------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/printk.h>

/* MACROS --------------------------------------------------------------------------------------- */

#define BT_UUID_CUSTOM_SERVICE_VAL \
  BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)

#define BLE_CUSTOM_SERVICE_MAX_DATA_LENGTH 20

/* PROTOTYPES ----------------------------------------------------------------------------------- */

static ssize_t ble_custom_service_read(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                                       void* buf, uint16_t len, uint16_t offset);
static ssize_t ble_custom_service_write(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                                        const void* buf, uint16_t len, uint16_t offset,
                                        uint8_t flags);

/* VARIABLES ------------------------------------------------------------------------------------ */

static const struct bt_uuid_128 ble_custom_service_uuid =
    BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_VAL);
static const struct bt_uuid_128 ble_custom_service_auth_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2));

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_CUSTOM_SERVICE_VAL),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static uint8_t ble_custom_service_auth_value[BLE_CUSTOM_SERVICE_MAX_DATA_LENGTH + 1] = {'E', 'i',
                                                                                        'E'};

/* BLE SERVICE SETUP ---------------------------------------------------------------------------- */

BT_GATT_SERVICE_DEFINE(ble_custom_service, BT_GATT_PRIMARY_SERVICE(&ble_custom_service_uuid),
                       BT_GATT_CHARACTERISTIC(&ble_custom_service_auth_uuid.uuid,
                                              BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE |
                                                  BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                                              ble_custom_service_read, ble_custom_service_write,
                                              ble_custom_service_auth_value),
                       BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE));

/* FUNCTIONS ------------------------------------------------------------------------------------ */

static ssize_t ble_custom_service_read(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                                       void* buf, uint16_t len, uint16_t offset) {
  const char* value = attr->user_data;

  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
}

static ssize_t ble_custom_service_write(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                                        const void* buf, uint16_t len, uint16_t offset,
                                        uint8_t flags) {
  uint8_t* value = attr->user_data;

  if (offset + len > BLE_CUSTOM_SERVICE_MAX_DATA_LENGTH) {
    printk("[BLE] ble_custom_service_write: Bad offset %d\n", offset + len);
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }

  memcpy(value + offset, buf, len);
  value[offset + len] = 0;

  printk("[BLE] ble_custom_service_write (%d, %d):", offset, len);
  for (uint16_t i = 0; i < len; i++) {
    printk("%s %02X '%c'", i == 0 ? "" : ",", value[offset + i], value[offset + i]);
  }
  printk("\n");

  return len;
}

static void ble_custom_service_notify(const void* const data, const uint16_t length) {
  bt_gatt_notify(NULL, &ble_custom_service.attrs[2], data, length);
}

/* MAIN ----------------------------------------------------------------------------------------- */

int main(void) {
  int err = bt_enable(NULL);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
    return 0;
  } else {
    printk("Bluetooth initialized!\n");
  }

  err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
  if (err) {
    printk("Advertising failed to start (err %d)\n", err);
    return 0;
  }

  int count = 0;
  while (1) {
    k_sleep(K_MSEC(1000));
    ble_custom_service_notify(&count, 4);
    count++;
  }
}
