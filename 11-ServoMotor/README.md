# QNX Servo Motor PWM Control Application

## Overview

This application demonstrates controlling an SG90 servo motor on QNX
using the `rpi_gpio` library.

The servo position is controlled by generating a PWM signal on a GPIO
pin.

The application: - Configures a GPIO pin as PWM output - Uses a 50Hz PWM
signal - Converts servo angles (0-180 degrees) into PWM duty cycle
values - Rotates the servo from 0 to 180 degrees and back

---

## Hardware Connection

SG90 Servo:

    Red       -> 5V
    Brown     -> GND
    Orange    -> GPIO18 PWM signal

---

## Servo PWM Principle

The SG90 uses a 50Hz PWM signal.

    Period = 20ms

Typical pulse widths:

    0 degree    -> 1ms pulse  (~2.5% duty cycle)
    90 degree   -> 1.5ms pulse
    180 degree  -> 2ms pulse  (~12.5% duty cycle)

The application maps:

    Angle
      |
      v
    Duty Cycle
      |
      v
    PWM Signal
      |
      v
    Servo Position

---

# PWM Initialization

The servo GPIO is configured:

``` c
rpi_gpio_setup_pwm(
    GPIO18,
    50,
    GPIO_PWM_MODE_MS);
```

This creates:

    GPIO18
     |
     PWM 50Hz
     |
     SG90 Servo

The MS mode keeps the PWM signal suitable for servo control.

---

# Setting Servo Angle

The function:

``` c
set_servo_angle()
```

converts an angle into duty cycle.

Example:

    0 degree:
    2.5% duty cycle

    90 degree:
    7.5% duty cycle

    180 degree:
    12.5% duty cycle

Then:

``` c
rpi_gpio_set_pwm_duty_cycle()
```

updates the PWM output.

---

# Program Flow

    Start
     |
     |
    Initialize PWM
     |
     |
    Set angle = 0
     |
     |
    Increase angle
     |
     |
    Move servo
     |
     |
    Delay 500ms
     |
     |
    Reach 180 degrees
     |
     |
    Decrease angle
     |
     |
    Move servo back
     |
     |
    End

---

# Rotation Logic

Forward rotation:

    0
     |
    10
     |
    20
     |
    ...
     |
    180

Backward rotation:

    180
     |
    170
     |
    160
     |
    ...
     |
    0

The step size is controlled by:

``` c
DEGREES_STEP
```

---

# QNX Design

The application acts as a client of the GPIO driver.

    Application

          |
          | rpi_gpio API
          v

    GPIO Driver

          |
          v

    PWM Hardware

          |
          v

    Servo Motor

The application does not directly access hardware registers.

---

# Important Parameters

## Frequency

``` c
50 Hz
```

Standard servo frequency.

## Duty Cycle Range

    2.5%  -> 0 degree

    12.5% -> 180 degree

## Step Delay

``` c
500ms
```

Controls rotation speed.

---

# Summary

This application demonstrates real-time servo control on QNX using
hardware PWM.

The GPIO driver generates the PWM signal, while the application controls
the servo position by changing the PWM duty cycle.
