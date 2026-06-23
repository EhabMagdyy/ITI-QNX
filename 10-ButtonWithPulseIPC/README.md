# QNX GPIO Event Driven LED Button Application

## Overview

This application demonstrates an event-driven GPIO design on QNX using
the `rpi_gpio` library.

A button connected to a GPIO input generates an interrupt event. The
GPIO driver sends a QNX pulse to the application, and the application
toggles an LED connected to another GPIO output.

The application does not continuously poll the button. Instead, it
sleeps waiting for an event.

---

## System Architecture

    Button
      |
      | GPIO interrupt
      v
    rpi_gpio Driver
      |
      | QNX Pulse IPC
      v
    Application Thread
      |
      v
    LED GPIO Output

---

## Hardware Connections

### LED

    GPIO16  ---> LED
    GND     ---> LED GND

### Button

    GPIO20  ---> Button
    GND     ---> Button

The button uses the internal pull-up resistor.

---

# QNX IPC Communication

The application creates a private channel:

``` c
chid = ChannelCreate(_NTO_CHF_PRIVATE);
```

The channel is used as a mailbox to receive events.

A connection is created:

``` c
coid = ConnectAttach(...);
```

The connection ID is given to the GPIO driver so it can send pulses.

---

## IPC Flow

    Application

    ChannelCreate()
          |
          |
         chid
          |
          v

    GPIO Driver

          |
          |
         coid
          |
          v

    MsgReceivePulse()

---

# Event Registration

The button is configured as an input:

``` c
rpi_gpio_setup_pull(
    GPIO20,
    GPIO_IN,
    GPIO_PUD_UP);
```

The application registers for GPIO changes:

``` c
rpi_gpio_add_event_detect(
    GPIO20,
    coid,
    GPIO_RISING | GPIO_FALLING,
    EVENT_BUTTON_1);
```

Now the driver monitors the GPIO pin.

---

# Pulse Handling

The main thread waits:

``` c
MsgReceivePulse(
    chid,
    &pulse,
    sizeof(pulse),
    NULL);
```

The thread blocks here until an event arrives.

When the button changes:

    Button press
         |
         v
    GPIO interrupt
         |
         v
    Driver creates pulse
         |
         v
    MsgReceivePulse returns

---

# LED Control

The LED GPIO is configured:

``` c
rpi_gpio_setup(
    GPIO16,
    GPIO_OUT);
```

Turn ON:

``` c
rpi_gpio_output(
    GPIO16,
    GPIO_HIGH);
```

Turn OFF:

``` c
rpi_gpio_output(
    GPIO16,
    GPIO_LOW);
```

---

# Button Debouncing

Mechanical buttons can generate multiple transitions:

    LOW
    HIGH
    LOW
    HIGH
    LOW

for one press.

The application ignores events that occur within 100ms:

``` c
debounce_threshold = 100000000;
```

This prevents multiple LED toggles from one press.

---

# Main Application Flow

    Start
     |
     |
    Create QNX channel
     |
     |
    Initialize button interrupt
     |
     |
    Initialize LED output
     |
     |
    Wait for pulse
     |
     |
    Button event received
     |
     |
    Toggle LED
     |
     |
    Wait again

---

# Important QNX Concepts

## Channel

A QNX IPC endpoint used for receiving messages or pulses.

Created with:

``` c
ChannelCreate()
```

---

## Connection ID

A handle used to send events/messages to a channel.

Created with:

``` c
ConnectAttach()
```

---

## Pulse

A lightweight asynchronous notification mechanism.

Used here for GPIO interrupts.

---

# Design Pattern

This application follows:

    Event Driven Programming

    Hardware Event
           |
           v
    Driver Notification
           |
           v
    Application Handler
           |
           v
    Action

Advantages:

-   No CPU wasting polling loop
-   Fast response to hardware events
-   Fits QNX real-time design
-   Driver owns hardware access

---

# Summary

The application acts as a GPIO client.

The `rpi_gpio` driver controls the hardware and notifies the application
using QNX pulses.

The application only handles events and decides what action to perform.
