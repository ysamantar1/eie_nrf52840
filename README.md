# BLE Security Demonstration
This project demonstrates Bluetooth Low Energy (BLE) secure pairing using a Nordic nRF52840 Development Kit, built with Zephyr and coded in C. When a phone connects to the device, a 6-digit passkey appears simultaneously on the LCD and serial console. The user must confirm the passkey on their phone when pairing in order to complete authentication and ensure a secure connection is made. This specifically demonstrates a level 4 connection which is both encrypted and authenticated and uses Elliptic Curve Diffie Hellman (ECDH).

This project is a part of Embedded in Embedded (EiE) and Math 518. EiE is a mentoring program for university engineering students that was started in 2002 at the University of Calgary. The program connects industry engineers with students interested in embedded systems. Math 518 is a thesis course for math students at the university. 

More to come on the mathematical implementation and background of BLEs and ECDH.

## Hardware & Firmware
This project is using the nRF52840 development kit from Nordic Semiconductors which uses the nRF52840 system on chip. The firmware used for this is based on the Zephyr RTOS and is contained within this repository.

## Setting Up
To set up everything, the following is helpful:
- [Getting Started](doc/1_Getting_Started/README.MD)
In order to compile the code, copy and execute the following:
`west build -b nrf52840dk/nrf52840 --shield adafruit_2_8_tft_touch_v2 app`
In order to run the code, use the following:
`west flash`

## Usage for L4
This is the default for this program.
 
### 1. Flash and open serial monitor
```
[ADV] Advertising as "nRF52840 SecureDemo".
[ADV] Passkey will appear on LCD and serial console.
```
The LCD shows a **blue screen**: *"BLE SecureDemo"*
 
### 2. Connect via nRF Connect
Open the app → SCAN → find **"nRF52840 SecureDemo"** → CONNECT.
 
LCD turns **yellow**: *"Connected! Waiting for pairing request..."*
 
### 3. Initiate pairing
In nRF Connect, accept the pairing prompt or tap the pair button.
 
LCD turns **dark blue with large white digits**:
```
  123456
  Match this passkey
  on your phone
  (nRF Connect)
```
 
Console shows the same:
```
============================================
  NUMERIC COMPARISON
  Passkey: 123456
  (Auto-confirming on device side)
============================================
```
 
### 4. Confirm passkey on phone
Confirm the 6-digit code shown on the LCD into nRF Connect.
 
### 5. Pairing complete
LCD turns **green**: *"Paired! Secure link active."*

## Usage for L3
Since this program is using L4 (instructions to use are above), there is a way to test out L3: Passkey Entry. The changes are as followed:
Set the following in the proj.conf file:
`
CONFIG_BT_SMP_SC_ONLY=n
CONFIG_BT_SMP_SC_PAIR_ONLY=n
`
as they are set to y to enforce LSE

Change `bt_conn_set_security(conn, BT_SECURITY_L4)` to `bt_conn_set_security(conn, BT_SECURITY_L3)` to ensure l3 is being implemented.

Finally, set `.passkey_confirm = NULL` and `.passkey_display = auth_passkey_display` in the bt_conn_auth_cb auth_cb struct as it ensures that passkey entry would be used instead of NC.

The instructions to use this are the same as L4, except a passkey is entered instead of matched.

### Schematic and Resources

- [Datasheet](https://docs.nordicsemi.com/bundle/ps_nrf52840/page/keyfeatures_html5.html)
- [Board Schematic, Layout drawings, and Altium files](https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/dev-kits/nrf52840-dk/nrf52840-development-kit---hardware-files-3_0_3.zip)
- [EiE](https://github.com/eiefirmware/eie_nrf52840)
