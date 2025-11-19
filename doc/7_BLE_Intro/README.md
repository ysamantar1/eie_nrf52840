# BLE Intro

[TOC]

## Introduction

One of the most powerful things that computers can do is communicate information information between them.
Typically, this communication is done digitally,
where bits and bytes are encoded and sent across some physical media,
such as electrical conductors (wires, traces, etc.), optical fibres, or what we're going to be focussing on, RF (radio frequencies).
In this program, we will be using the BLE capabilities of the nRF52840DK board to wirelessly send and receive data with other devices.

## Motivation

At the embedded level,
wireless communication can be very helpful,
since a lot of embedded devices are very physically IO limited.
However, wireless communication is naturally a very power-hungry thing to do,
which can be an issue, since embedded devices are frequently power constrained,
usually being reliant on batteries that are intended to last for days, weeks, months, or even years at a time.

This is where Bluetooth Low Energy (BLE) comes in.
BLE provides a power-efficiency-focused,
standardized communication mechanism that is meant to be used by devices of all different uses,
regardless of the manufacturer.
There are a few other alternatives in the wireless communication space
(WiFi, Bluetooth Classic, ANT, Zigbee, etc.) that have different focuses,
but BLE provides an excellent base what we're doing.

## Lesson

### Explore the Basics of BLE

While we will be giving a brief introduction to the basics of BLE over the next few lessons,
a much more thorough introductory course has been made freely accessible by Nordic
(who make the radios and boards we're using):
<https://academy.nordicsemi.com/courses/bluetooth-low-energy-fundamentals/>

The above course covers a lot of content, so finishing it in one sitting is not practical,
especially in just a single EiE session.
For today, it will be worthwhile to only go over
[Lesson 1: Bluetooth LE Introduction](https://academy.nordicsemi.com/courses/bluetooth-low-energy-fundamentals/lessons/lesson-1-bluetooth-low-energy-introduction/).
The two main sections to focus on will be GAP ("Generic Access Profile") and GATT ("Generic Attribute Profile"),
which define how almost all BLE communication happens.

As for the rest of the lessons,
it would be useful to have the course open and peruse through it when you want more information on a specific topic. 

#### Goals

As a check to see if you're ready to move on, see if you can answer the following questions:

1. If device A searches for and connects to device B, which device must be *advertising*?
<!-- Answer: Device B must be advertising -->
2. In question 1, which device(s) are in the *central* role? Which device(s) are in the *peripheral* role?
<!-- Answer: Device A is the central, device B is the peripheral -->
3. Can a device be acting as both a *central* and a *peripheral*?
<!-- Answer: Yes -->
4. If a device is in the *central* GAP role, what GATT roles (*client*, *server*) can it act as? What about the *peripheral* GAP role?
<!-- Answer: A device can act as both a GATT client and server, regardless of its GAP role -->
5. If a device is transmitting bike speed information over BLE, would the field that actually contains the numeric speed value be a *service*, *characteristic*, or *attribute*?
<!-- Answer: Characteristics are the structures that actually contain data. Services are groups of these characteristics, and attributes are metadata surrounding how characteristics act. -->

Of course, this isn't graded, so this "quiz" is only for you to judge your own understanding.
Answers can be found in the source text for this file, so aren't visible in the rendered output that you're probably reading.
**BLE is a** ***very*** **large and complicated protocol, so don't be discouraged if this seems like a lot of information.**
**If you are ever confused, ask a leader!**
**We're here to help, and if you're confused, someone else likely is too, and we can provide more information where needed.**

### Install nRF Connect

If you have access to a smartphone,
install the "nRF Connect" app by Nordic Semiconductor.
If you are unable to do this, pair up with someone around you that can,
otherwise ask a leader for alternatives.

This app is contains a collection of tools that allows for real-time viewing, analysis, and interaction with any BLE peripheral in the area.
When opened, you should see a list of nearby BLE devices start populating,
along with their advertising address (6 hexadecimal bytes separated by colons),
signal strength ("RSSI", measured in dBm),
and advertising interval (time between received "advertising packets", measured in milliseconds).
Some devices will have a "Connect" button too.

![](images/nRF_Connect.jpg)

Once you see this, you are ready to move on.

### Modify, Build, and Flash Example Code

The code in this repository is already setup to be a very basic BLE peripheral.
This uses a custom BLE service that has a single characteristic which can be ready from, written to, or send out notifications.
Let's modify, build, and flash this example to your board.
To do so, you must:

1. Open up a new Bash terminal and navigate to your `~/zephyr-projects` directory.
2. Activate your virtual environment with `source .venv/Scripts/activate` (Windows) or `source .venv/bin/activate` (Mac/Linux).
3. Navigate into your `eie_nrf52840.git` directory and open VS Code via `code .`.
4. Open the `prj.conf` file and change the `CONFIG_BT_DEVICE_NAME` string to be `"#### EiE BLE Peripheral"`, where `####` is a number unique to you, like the last 4 digits of your UCID. This is important, since when everyone's devices start advertising, they'll all show up in your nRF Connect too!
5. Go back to your Bash terminal and build the project with `west build -p always -b nrf52840dk/nrf52840 app`
6. Plug in your dev board and flash it with `west flash`

If there were no error messages, you've flashed your board!
Now time to move on and connect with it via nRF Connect.

### Connect via nRF Connect

Once your board has been flashed with the BLE peripheral example code,
it is set to start advertising and become receptive to connections on startup.

1. Open up nRF Connect on your phone.
2. Expand the "Filter" field near the top,
   set the RSSI limit to -40 dBm (all the way to the left),
   and then tap the 3 dots on the "Exclude" field and check every box.
   These settings will make it easier to see only the boards near you.

![](images/search_filters.jpg)

3. Refresh the search by swiping down, just like you would a web page.

![](images/found_sensor.jpg)

4. Hit the "Connect" button on your board's entry in order to connect to it.
   This should take you to a new tab with more details about your device.
5. Verify that you see "Generic Attribute", "Generic Access", and "Unknown Service" entries, like so:

![](images/connected.jpg)

6. Tap on "Unknown Service" (this is our custom service that is made by the example code).
   Ensure that you see a single "Unknown Characteristic" with the "READ", "WRITE", and "NOTIFY" properties,
   as well as three different arrow-based icons, like so:

![](images/service.jpg)

7. Test read (should be "EiE")
8. Test write (output via printk)
9. Test notify (counter at 1Hz, note that it counts up regardless of if central is subscribed)

Note that there is no proper disconnect handling.
The board will connect to only the first device that tries, and only the first time.
For now, this prevents others from connecting to your board once you have already,
but we will make this more elegant in the future.
As a result, **you have to reset the dev board in order for a device to reconnect ove BLE**,
simply hit the reset button on the board or unplug it and plug it back in.

### Explore the Example Code

- Service description macro
- Read callback
- Write callback
- Notify function
- Main
  - BLE init
  - Notify function called in main loop

## Challenge

- Make it so that:
   - Writing "LED ON" or "LED OFF" to the characteristic sets LED 1 appropriately
   - Pushing Button 1 swaps the direction of the notify counter
