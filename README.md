# tribit32-advanced

A tri-microcontroller, overengineered GameBoy Advance clone. More value was given to the controls and the exterior look, which is cool!

## Introduction

This guide is more about the software side of the project. The 3D-Designs and the KiCad files can be found in their respective folders and be edited as needed. This project is as open source as I can make it to be, so if something is missing, message me and I will try to add it here. It would still like builds using this guide to mention me and link to this guide, so thanks in advance!

## Electronics

This section includes information about the electronics I used for the project. This project can be built in three ways: just a single board computer, just the microcontroller, or both at once. Doing both increases difficulty exponentially, so it is not really recommended and is more of a showpiece. 

### Microcontrollers and Single Board Computers

The custom PCB is laid out for an ESP32-S3-DevkitC and Raspberry Pi Zero 2W. If you can make alternatives work with adapters / custom software, you can use that too. However this project focuses more on these two specific boards. 

If you are going to be including a Raspberry Pi, you will also need to get an Arduino Pro Mini for the controller emulation. 

### Periphery

- **ILI9341 3.2inch *non-touch* SPI LCD**: 320x240 16 Bit Color, rated for 10 MHz SPI but most comfortably can run at ~67 MHz. ST7789 might work, but definitely not tested.
- **MAX98357A I2S DAC**: A pretty ok DAC + Amplifier combo. PCM5102 might work too, but these modules are cheap anyway.
- **Micro SD Adapter**: Only needed for ESP32. 
- **2x 4 ohm 20x30 mm Speakers**: Connected in series they enable rich sound.

### Power

- **5 V / 3.3 V Regulators**: Please do not use LDOs or linear regulators. These are inneficient. If just running the Raspberry Pi you can technically forego the 3.3 V Regulator and just use a 5 V one, as the Pi already has (a weak) one on board. Or even better? Use an MH-CD42 Li-Ion charger module with the onboard regulator. Less parts mean less things to solder!
- **Li-Ion Charger**: I originally used a TP4056 which did technically work fine, but after finishing the project I found out better alternatives exist that dont really have that much difference in price. Just get an MH-CD42!
### Other Components

- **6 mm Silicone Buttons**: You can also use different alternatives, the PCB should be able to support them with minimal modifications to the buttons.
- **100 ohm, 10 kohm and 100 kohm Resistors**: Values are actually not that important, something in the ballpark should work. Used mainly for pullups, resistor divider for battery monitoring...
- **Wires**: Use what you have...
- **2.54 mm male and female headers**: You can also totally do the project without any female or even any headers, but you can make pretty much everything dissasemble-able using these. 
- **BC547 NPN Transistors**: Really only needed if you are building a two-board system. 

### PCB

![PCB Front Side](https://github.com/ohasanov-hbrw/tribit32-advanced/blob/master/Images/PCBFrontWhite.jpg?raw=true)
![PCB Back Side](https://github.com/ohasanov-hbrw/tribit32-advanced/blob/master/Images/PCBBackWhite.jpg?raw=true)

This project is mainly thought out to be built with a custom PCB. Although it is technically possible to make this with normal perfboards as all used components are through hole components (no SMD components), a PCB makes the assembly process a lot faster whilst also resulting in a cleaner build. 

The PCBs for my build were manufactured by [NextPCB](https://www.nextpcb.com/). Thanks a lot for them sending over the PCBs and thus motivating me to design my own PCBs for my future projects. 



## Software Guide

This section will be divided into two subsections. Software for the ESP32 and Software for the Raspberry Pi