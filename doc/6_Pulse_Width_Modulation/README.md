# Pulse Width Modulation

## Table of Contents
- [Pulse Width Modulation](#pulse-width-modulation)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Motivation](#motivation)
  - [How does it work?](#how-does-it-work)
  - [How will we be using it?](#how-will-we-be-using-it)
  - [Lesson](#lesson)
  - [Exercises](#exercises)

## Introduction

This lesson will cover *Pulse Width Modulation*, which is usually abbreviated to *PWM*. PWM is a very
common technique used in embedded systems to emulate an analog output with only digital pins.

## Motivation

Computers are digital, but the world is analog. Because of this, embedded systems need to both
measure and produce analog signals with only digital circuitry at their disposal. Chips often
produce analog signals using digital to analog converters, but these are expensive in terms of
silicon area, power consumption and added hardware complexity. Enter pulse width modulation, a
technique to produce "analog" signals using a basic GPIO pin and a hardware timer. This drastically
reduces the aforementioned costs and is good enough in many applications!

## How does it work?

The definition of PWM is the method of sending a series of pulses where the width (the on-time) of
the pulses can be varied. The average voltage seen by the load is equal to the ratio of the pulse
width to the pulse period, multiplied by the pulse amplitude.

$$
V_{avg} = T_{width} / T_{period} * V_{max}
$$

The ratio of pulse width to pulse period expressed as a percentage is called the *duty cycle*

For example, if you send a series of 1V pulses with a 10ms period and a 5ms on-time, the average
voltage seen by the load will be 0.5V! Here, the pulse width refers to the "on-time" of the pulse's
period. The ratio of the pulse width to the pulse period is 5ms / 10ms, or 0.5 in this case, 
therefore the average voltage seen by the load is 0.5 * 1V or 0.5V (as seen below)  
![](images/PWM_1V_10ms_50.png)  
This PWM setting has a duty cycle of 50 %

Take another example, let's reduce the pulse width to just 20% of the pulse's overall period. With
this setup, our average load voltage should be 2ms / 10ms * 1V or 0.2V:  
![](images/PWM_1V_10ms_20.png)  
This PWM setting has a duty cycle of 20%

For one last example, let's increase the pulse width all the way to 80% of the pulse's overall
period. In this case, the average load voltage will be 8ms / 10ms * 1V or 0.8V!  
![](images/PWM_1V_10ms_80.png)  
This PWM setting has a duty cycle of 80%

As you can see, we can vary the average voltage seen by the load just by varying how long the pulse
is on for! We can even add a capacitor to the output of the PWM to get a smoother signal, note that
this isn't always done and can delay the response of the signal:  
![](images/PWM_1V_10ms_50_smoothed.png)  

## How will we be using it?

Human eyes can only track changes happening as fast as ~30Hz, and above that frequency (known as the 
flicker-fusion threshold) LED flashes fuse and can't be distinguished. This means that we can use PWM 
to vary how bright an LED appears. Think of our eyes as the capacitor in this case, and that their
inability to detect rapid changes "smoothes" the LED to look more or less bright.

Because we need to be well above the flicker-fusion threshold for the LED to appear smooth and not
flickering, PWM signals controlling LED brightness are usually between 50 - 100Hz.

As an example, say we power an LED using a PWM signal with a duty cycle of 50%, the LED will appear to
be approximately half as bright as if we gave it full power. It's important to note that this doesn't
mean that the LED is actually on at half the brightness - the LED is turning fully on and fully off so
quickly that it only appears to be half as bright as our eyes blur the rapid changes.

It's important to note that PWM is used in many applications beyond simple LED brightness, for example:
- Servo motor control
- Voltage regulation
- Buzzer control

## Lesson

We've added a new API to the LED module that you can now use to turn on an LED at a given PWM duty cycle:  
`int LED_pwm(led_id led, uint8_t duty_cycle)`: sets the LED to the given duty cycle. Note that the duty_cycle
argument accepts values between 0 and 100. Returns 0 on success and a negative error code on failures.

## Exercises

