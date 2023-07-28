# SerialBlaster
SerialBlaster is built for the [M5StickC](https://m5stack.com/products/stick-c). It allows IR commands to be sent to the M5StickC over USB using serial RS232, which will then in turn be blasted from the built in IR blaster. The LCD screen is used as a debug aid to view the serial messages being sent to the device.

<img width="250" alt="portfolio_view" src="https://shop.m5stack.com/cdn/shop/products/01_1200x1200.jpg?v=1587104211">


## Build Requirements
* Arduino Development Environment with M5StickC library

## Default Serial Port Parameters
* Baud Rate: **115200**
* Data Bits: **8**
* Parity: **None**
* Stop Bits: **1**

## Example Serial Commands
* Send NEC code 0x3EC14DB2: ```send nec 0x3EC14DB2\r```
* Print "Hello World" to LCD: ```message Hello World\r```

## Controls
* Home Button: **Toggle LCD screen on or off**
* Reset Button: **Reset**

## Future Work
* Add support for other IR protocols such as _Sony_.
