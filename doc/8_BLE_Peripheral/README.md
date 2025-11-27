# BLE Peripheral

[TOC]

## Introduction

As an extension of the last lesson,
we're going to now go over building a custom BLE peripheral from scratch.
We'll be keeping the peripheral as a GATT server,
meaning that it will be the device that hosts the services and characteristics
presented to connecting devices.

## Lesson

For this lesson,
we'll be trying to replicate and expand on the BLE peripheral example from the last lesson.

### Kconfig Project Configuration

In order to enable the BLE peripheral section of the Zephyr RTOS,
we need to set a few configuration values in the `prj.conf` file:

1. `CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE` -
    This allows for the operating system's internal event handler to allocate more memory for its stack.
    Because we are requesting the OS to control more complicated behaviour than normal, we need to increase this value.
    Set this to `2048` (measured in bytes).
2. `CONFIG_BT` -
    This config simply controls whether or not BLE in general is enabled.
    Set this to `y` (enabled).
3. `CONFIG_BT_PERIPHERAL`
    This config enables the peripheral mode of the BLE subsystem.
    With this enabled, we get the ability to tell the device to advertise, and thus, have other devices connect to our board.
    Set this to `y` (enabled).
4. `CONFIG_BT_DEVICE_NAME` -
    Finally, this is the name that the device will advertise with.
    Set this to something unique to you, such as `"#### EiE BLE Peripheral"`, where `####` is the last 4 digits of your UCID.

Making these changes in `prj.conf` should still allow your project to compile,
even before you make any code changes.
To make sure you've done the modifications correctly, make sure your project still compiles! 

### Source Code Changes

#### UUIDs

Every custom BLE service and characteristic requires a "Universally Unique Identifier" (UUID).
These are 16B numbers that are ideally completely unique in a given context.
For our purposes, we can set these to whatever we want,
but you'll want to make sure you never use the same UUID twice for two different items.

You can use the `BT_UUID_128_ENCODE` macro to define two UUIDs,
one for a service, and one for a characteristic.
For ease of use, use `#define`s for this.

```c
BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)
```

#### Advertising Data

We have to define what data the radio sends out in advertising packets.
We can generally put whatever information we want in the advertising data as long as the whole packet is at most 31 bytes,
however specific things like connection flags or available service IDs aid in discovery and connection forming.
There are some features to increase this size to up to 1650 bytes, but we won't be using them,
as the focus of our advertising is only going to be for connection establishment.
We're going to be using a data flags field (3B) and our device name (variable size).

We can generate an encoding for the flags field with `BT_DATA_BYTES(BT_DATA_FLAGS, flags)`,
where `flags` is a series of bitwise-OR'd values that set specific bits.
For our use, we need to set the `BT_LE_AD_GENERAL` and `BT_LE_AD_NO_BREDR` flags.
`BT_LE_AD_GENERAL` enables "general discoverability mode",
allowing any device to receive our advertising packets and try to connect,
and `BT_LE_AD_NO_BREDR` simply tells the connecting devices that we only support BLE,
not the "classic" BLE (a.k.a. "BREDR").

Next we can include our device name with `BT_DATA(BT_DATA_NAME_COMPLETE, name, sizeof(name))`,
where `name` is an ASCII array (e.g. string).
Since we've defined our device name with Kconfig,
we can use `CONFIG_BT_DEVICE_NAME` for this.

All we have to do now is define an `bt_data` array to use as our advertising packets:

```c
static const struct bt_data ble_advertising_data[] = {
  BT_DATA_BYTES(...),
  BT_DATA(...),
};
```

#### Characteristic Data Buffers

For each of our characteristics,
we need to define an array that stores the data written to or read from it.

We should set the maximum size for this array to be 20B in order to work with the default parameters available to most devices.
As with advertising data, there are mechanisms to expand this, however we won't be using them.


```c
static uint8_t ble_custom_characteristic_user_data[20] = {};
```

#### Read Callback

We need to define a callback for when the characteristic is read from.
In its simplest form, we simply relay the read event to the GATT server,
which then will send the current stored value of the characteristic to the connected device.

```c
static ssize_t ble_custom_service_read(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                                       void* buf, uint16_t len, uint16_t offset) {
  const char* value = attr->user_data;
  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
}
```

#### Write Callback

Similarly to the read callback,
we need to define a callback for when our characteristic is written to.
In its simplest form, all we do is take the value written, ensure that it isn't too long,
and then save it into the characteristic's data buffer.

```c
static ssize_t ble_custom_service_write(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                                        const void* buf, uint16_t len, uint16_t offset,
                                        uint8_t flags) {
  uint8_t* value_ptr = attr->user_data;

  if (offset + len > BLE_CUSTOM_CHARACTERISTIC_MAX_DATA_LENGTH) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }

  memcpy(value_ptr + offset, buf, len);
  value_ptr[offset + len] = 0;

  return len;
}
```

#### Service and Characteristic Description

Now we can tie all of our pieces together with the service definition macro, `BT_GATT_SERVICE_DEFINE`.
This is what actually creates the service and its included characteristics.
Inside of this macro,
we first specify a name for the created object that stores all of the config for our service,
the the UUID for the service,
and finally the characteristics.

First, let's define `bt_uuid_128` objects for each of out UUIDs,
where `BLE_CUSTOM_SERVICE_UUID` and `BLE_CUSTOM_CHARACTERISTIC_UUID` are the `#define`s from earlier:

```c
static const struct bt_uuid_128 ble_custom_service_uuid = BT_UUID_INIT_128(BLE_CUSTOM_SERVICE_UUID);
static const struct bt_uuid_128 ble_custom_characteristic_uuid =
    BT_UUID_INIT_128(BLE_CUSTOM_CHARACTERISTIC_UUID);
```

Then we'll start the service definition macro:

```c
BT_GATT_SERVICE_DEFINE(
    ble_custom_service,  // Name of the struct that will store the config for this service
    BT_GATT_PRIMARY_SERVICE(&ble_custom_service_uuid),  // Setting the service UUID
    ...
);
```

Finally, we can define our characteristic using the `BT_GATT_CHARACTERISTIC` macro.
In order, we need to pass the characteristic UUID,
permitted operations,
default permissions,
callbacks,
and a location for it to store its data.
Most of these we've already defined,
but the permitted operations and the default permissions haven't been touched yet.
These all use either pre-defined `BT_GATT_CHRC_*` (for permitted operations) or `BT_GATT_PERM_*` (for permissions)
values all bitwise-OR'd together.
This sets specific flag bits that the BLE stack uses to configure some lower-level behaviour.
For both of these, we want the `READ` and `WRITE` values.

```c
    ...
    BT_GATT_CHARACTERISTIC(
        &ble_custom_characteristic_uuid.uuid,  // Setting the characteristic UUID
        <BT_GATT_CHRC_* values ORd together>,  // Possible operations
        <BT_GATT_PERM_* values ORd together>,  // Permissions that connecting devices have
        ble_custom_service_read,             // Callback for when this characteristic is read from
        ble_custom_service_write,            // Callback for when this characteristic is written to
        ble_custom_characteristic_user_data  // Initial data stored in this characteristic
        ),
    ...
```

#### Main

TODO

```c
int main(void) {
  int err = bt_enable(NULL);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
    return 0;
  } else {
    printk("Bluetooth initialized!\n");
  }

  err =
      bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ble_advertising_data, ARRAY_SIZE(ble_advertising_data),
                      ble_scan_response_data, ARRAY_SIZE(ble_scan_response_data));
  if (err) {
    printk("Advertising failed to start (err %d)\n", err);
    return 0;
  }

  while (1) {
    k_sleep(K_MSEC(1000));
    ble_custom_service_notify();
  }
}
```
