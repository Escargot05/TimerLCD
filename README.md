# Timer with LCD display 

## Overview

This is my first project on Arduino platform. The timer iplementation was made througth "bare metal" approach. To control display the LiquidCrystal 1.5 library was used.

## Hardware

* Arduino UNO R3
* LCD display (HD44780)
* yellow LED diode
* 4x buttons 
* 4x 10kOhm resistor


## Electrical scheme



## Detailed description

The timer count down the time from the set value to 0. The LCD displayes minutes and seconds values. When time is equal to 0 then the alarm is turned on by lighting yellow diode. Turning off the alarm can be done by pressing the "clr / start" button. The initial value is set using the "+", "-" and "sel" buttons. After setting the initial value, the "clr / start" button starts the countdown process.

## Preview

