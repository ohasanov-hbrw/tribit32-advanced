# Modifications

This was also forked from [Original Modified Repo](https://github.com/The-Next-Guy/fbcp-nexus/). This fork has some modifications to fit better with my gameboy build. Modified by ohasanov.

## Original Repo

This was forked from the [Original Repo](https://github.com/juj/fbcp-ili9341). Some of the original README.md was modified. I preserved building informations, but removed stats and FAQ. To see the original information, go to the original repo. This repository implements a driver for certain SPI-based LCD displays for Raspberry Pi A, B, 2, 3, 4 and Zero. Modified by The_Next_Guy


### Tested Devices

The driver has been checked to work (at least some point in the past) on the following systems:

 - Raspberry Pi 3 Model B+ with Raspbian Stretch (GCC 6.3.0)
 - Raspberry Pi 3 Model B Rev 1.2 with Raspbian Jessie (GCC 4.9.2) and Raspbian Stretch (GCC 6.3.0)
 - Raspberry Pi Zero W with Raspbian Jessie (GCC 4.9.2) and Raspbian Stretch (GCC 6.3.0)
 - Raspberry Pi 2 Model B
 - Raspberry Pi B Rev. 2.0 (old board from Q4 2012, board revision ID 000e)
 - Raspberry Pi Zero W 2 with RetroPie

### Installation

Check the following sections to set up the driver.

##### Boot configuration

This driver does not utilize the [notro/fbtft](https://github.com/notro/fbtft) framebuffer driver, so that needs to be disabled if active. That is, if your `/boot/config.txt` file has lines that look something like `dtoverlay=pitft28r, ...`, `dtoverlay=waveshare32b, ...` or `dtoverlay=flexfb, ...`, those should be removed.

This program neither utilizes the default SPI driver, so a line such as `dtparam=spi=on` in `/boot/config.txt` should also be removed so that it will not cause conflicts.

Likewise, if you have any touch controller related dtoverlays active, such as `dtoverlay=ads7846,...` or anything that has a `penirq=` directive, those should be removed as well to avoid conflicts. It would be possible to add touch support to fbcp-ili9341 if someone wants to take a stab at it.

##### Building and running

Run in the console of your Raspberry Pi:

```bash
sudo apt-get install cmake
cd ~
git clone https://github.com/The-Next-Guy/fbcp-nexus.git
cd fbcp-nexus
mkdir build
cd build
cmake [options] ..
make -j
sudo ./fbcp-ili9341
```

Note especially the two dots `..` on the CMake line, which denote "up one directory" in this case (instead of referring to "more items go here").

See the next section to see what to input under **[options]**.

If you have been running existing `fbcp` driver, make sure to remove that e.g. via a `sudo pkill fbcp` first (while running in SSH prompt or connected to a HDMI display), these two cannot run at the same time. If `/etc/rc.local` or `/etc/init.d` contains an entry to start up `fbcp` at boot, that directive should be deleted.

##### Configuring build options

There are generally two ways to configure build options, at CMake command line, and in the file [config.h](https://github.com/juj/fbcp-ili9341/blob/master/config.h).

On the CMake command line, the following options can be configured:

###### If you have a display Add-On Hat

When using one of the displays that stack on top of the Pi that are already recognized by fbcp-ili9341, you don't need to specify the GPIO pin assignments, but fbcp-ili9341 code already has those. Pass one of the following CMake directives for the hats:

- `-DADAFRUIT_ILI9341_PITFT=ON`: If you are running on the [Adafruit 2.8" 320x240 TFT w/ Touch screen for Raspberry Pi](https://www.adafruit.com/product/1601) (or the [Adafruit PiTFT 2.2" HAT Mini Kit - 320x240 2.2" TFT - No Touch](https://www.adafruit.com/product/2315) display, which is compatible), pass this flag.
- `-DADAFRUIT_HX8357D_PITFT=ON`: If you have the [Adafruit PiTFT - Assembled 480x320 3.5" TFT+Touchscreen for Raspberry Pi](https://www.adafruit.com/product/2097) display, add this line.
- `-DFREEPLAYTECH_WAVESHARE32B=ON`: If you are running on the [Freeplay CM3 or Zero](https://www.freeplaytech.com/product/freeplay-cm3-diy-kit/) device, pass this flag. (this is not a hat, but still a preconfigured pin assignment)
- `-DWAVESHARE35B_ILI9486=ON`: If specified, targets a [Waveshare 3.5" 480x320 ILI9486](https://www.amazon.co.uk/dp/B01N48NOXI/ref=pe_3187911_185740111_TE_item) display.
- `-DTONTEC_MZ61581=ON`: If you are running on the [Tontec 3.5" 320x480 LCD Display](https://www.ebay.com/p/Tontec-3-5-Inches-Touch-Screen-for-Raspberry-Pi-Display-TFT-Monitor-480x320-LCD/1649448059) display, pass this.
- `-DPIRATE_AUDIO_ST7789_HAT=ON`: If specified, targets a [Pirate Audio 240x240, 1.3inch IPS LCD display HAT for Raspberry Pi](https://shop.pimoroni.com/collections/pirate-audio) with ST7789 display controller
- `-DWAVESHARE_ST7789VW_HAT=ON`: If specified, targets a [240x240, 1.3inch IPS LCD display HAT for Raspberry Pi](https://www.waveshare.com/1.3inch-lcd-hat.htm) with ST7789VW display controller.
- `-DWAVESHARE_ST7735S_HAT=ON`: If specified, targets a [128x128, 1.44inch LCD display HAT for Raspberry Pi](https://www.waveshare.com/1.3inch-lcd-hat.htm) with ST7735S display controller.
- `-DKEDEI_V63_MPI3501=ON`: If specified, targets a [KeDei 3.5 inch SPI TFTLCD 480*320 16bit/18bit version 6.3 2018/4/9](https://github.com/juj/fbcp-ili9341/issues/40) display with MPI3501 display controller.

###### If you wired the display to the Pi yourself

If you connected wires directly on the Pi instead of using a Hat from the above list, you will need to use the configuration directives below. In addition to specifying the display, you will also need to tell fbcp-ili9341 which GPIO pins you wired the connections to. To configure the display controller, pass one of:

- `-DILI9341=ON`: If you are running on any other generic ILI9341 display, or on Waveshare32b display that is standalone and not on the FreeplayTech CM3/Zero device, pass this flag.
- `-DILI9340=ON`: If you have a ILI9340 display, pass this directive. ILI9340 and ILI9341 chipsets are very similar, but ILI9340 doesn't support all of the features on ILI9341 and they will be disabled or downgraded.
- `-DHX8357D=ON`: If you have a HX8357D display, pass this directive.
- `-DSSD1351=ON`: If you have a SSD1351 OLED display, use this.
- `-DST7735R=ON`: If you have a ST7735R display, use this.
- `-DST7789=ON`: If you have a ST7789 display, use this.
- `-DST7789VW=ON`: If you have a ST7789VW display, use this.
- `-DST7735S=ON`: If you have a ST7735S display, use this.
- `-DILI9486=ON`: If you have a ILI9486 display, pass this directive.
- `-DILI9486L=ON`: If you have a ILI9486L display, pass this directive. Note that ILI9486 and ILI9486L are quite different, mutually incompatible controller chips, so be careful here identifying which one you have. (or just try both, should not break if you misidentified)
- `-DILI9488=ON`: If you have a ILI9488 display, pass this directive.
- `-DMPI3501=ON`: If specified, targets a display with MPI3501 display controller.

And additionally, pass the following to customize the GPIO pin assignments you used:

- `-DGPIO_TFT_DATA_CONTROL=number`: Specifies/overrides which GPIO pin to use for the Data/Control (DC) line on the 4-wire SPI communication. This pin number is specified in BCM pin numbers. If you have a 3-wire SPI display that does not have a Data/Control line, **set this value to -1**, i.e. `-DGPIO_TFT_DATA_CONTROL=-1` to tell fbcp-ili9341 to target 3-wire ("9-bit") SPI communication.
- `-DGPIO_TFT_RESET_PIN=number`: Specifies/overrides which GPIO pin to use for the display Reset line. This pin number is specified in BCM pin numbers. If omitted, it is assumed that the display does not have a Reset pin, and is always on.
- `-DGPIO_TFT_BACKLIGHT=number`: Specifies/overrides which GPIO pin to use for the display backlight line. This pin number is specified in BCM pin numbers. If omitted, it is assumed that the display does not have a GPIO-controlled backlight pin, and is always on. If setting this, also see the `#define BACKLIGHT_CONTROL` option in `config.h`.

fbcp-ili9341 always uses the hardware SPI0 port, so the MISO, MOSI, CLK and CE0 pins are always the same and cannot be changed. The MISO pin is actually not used (at the moment at least), so you can just skip connecting that one. If your display is a rogue one that ignores the chip enable line, you can omit connecting that as well, or might also be able to get away by connecting that to ground if you are hard pressed to simplify wiring (depending on the display).

###### Specifying display speed

To get good performance out of the displays, you will drive the displays far out above the rated speed specs (the rated specs yield about ~10fps depending on display). Due to this, you will need to explicitly configure the target speed you want to drive the display at, because due to manufacturing variances each display copy reaches a different maximum speed. There is no "default speed" that fbcp-ili9341 would use. Setting the speed is done via the option

- `-DSPI_BUS_CLOCK_DIVISOR=even_number`: Sets the clock divisor number which along with the Pi [core_freq=](https://www.raspberrypi.org/documentation/configuration/config-txt/overclocking.md) option in `/boot/config.txt` specifies the overall speed that the display SPI communication bus is driven at. `SPI_frequency = core_freq/divisor`. `SPI_BUS_CLOCK_DIVISOR` must be an even number. Default Pi 3B and Zero W `core_freq` is 400MHz, and generally a value `-DSPI_BUS_CLOCK_DIVISOR=6` seems to be the best that a ILI9341 display can do. Try a larger value if the display shows corrupt output, or a smaller value to get higher bandwidth. See [ili9341.h](https://github.com/juj/fbcp-ili9341/blob/master/ili9341.h#L13) and [waveshare35b.h](https://github.com/juj/fbcp-ili9341/blob/master/waveshare35b.h#L10) for data points on tuning the maximum SPI performance. Safe initial value could be something like `-DSPI_BUS_CLOCK_DIVISOR=30`.

###### Specifying the target Pi hardware

There are a couple of options to explicitly say which Pi board you want to target. These should be autodetected for you and generally are not needed, but e.g. if you are cross compiling for another Pi board from another system, or want to be explicit, you can try:

- `-DSINGLE_CORE_BOARD=ON`: Pass this option if you are running on a Pi that has only one hardware thread (Pi Model A, Pi Model B, Compute Module 1, Pi Zero/Zero W). If not present, autodetected.
- `-DARMV6Z=ON`: Pass this option to specifically optimize for ARMv6Z instruction set (Pi 1A, 1A+, 1B, 1B+, Zero, Zero W). If not present, autodetected.
- `-DARMV7A=ON`: Pass this option to specifically optimize for ARMv7-A instruction set (Pi 2B < rev 1.2). If not present, autodetected.
- `-DARMV8A=ON`: Pass this option to specifically optimize for ARMv8-A instruction set (Pi 2B >= rev. 1.2, 3B, 3B+, CM3, CM3 lite, 4B, CM4, Pi400). If not present, autodetected.

###### Specifying other build options

The following build options are general to all displays and Pi boards, they further customize the build:

- `-DBACKLIGHT_CONTROL=ON`: If set, enables fbcp-ili9341 to control the display backlight in the given backlight pin. The display will go to sleep after a period of inactivity on the screen. If not, backlight is not touched.
- `-DDISPLAY_CROPPED_INSTEAD_OF_SCALING=ON`: If set, and source video frame is larger than the SPI display video resolution, the source video is presented on the SPI display by cropping out parts of it in all directions, instead of scaling to fit.
- `-DDISPLAY_BREAK_ASPECT_RATIO_WHEN_SCALING=ON`: When scaling source video to SPI display, scaling is performed by default following aspect ratio, adding letterboxes/pillarboxes as needed. If this is set, the stretching is performed breaking aspect ratio.
- `-DSTATISTICS=number`: Specifies the level of overlay statistics to show on screen. 0: disabled, 1: enabled, 2: enabled, and show frame rate interval graph as well. Default value is 1 (enabled).
- `-DUSE_DMA_TRANSFERS=OFF`: If specified, disables using DMA transfers (at great expense of lost CPU usage). Pass this directive if DMA is giving some issues, e.g. as a troubleshooting step if something is not looking right.
- `-DDMA_TX_CHANNEL=<num>`: Specifies the DMA channel number to use for SPI send commands. Change this if you find a DMA channel conflict.
- `-DDMA_RX_CHANNEL=<num>`: Specifies the DMA channel number to use for SPI receive commands. Change this if you find a DMA channel conflict.
- `-DDISPLAY_SWAP_BGR=ON`: If this option is passed, red and blue color channels are reversed (RGB<->BGR) swap. Some displays have an opposite color panel subpixel layout that the display controller does not automatically account for, so define this if blue and red are mixed up.
- `-DDISPLAY_INVERT_COLORS=ON`: If this option is passed, pixel color value interpretation is reversed (white=0, black=31/63). Default: black=0, white=31/63. Pass this option if the display image looks like a color negative of the actual colors.
- `-DDISPLAY_ROTATE_180_DEGREES=ON`: If set, display is rotated 180 degrees. This does not affect HDMI output, only the SPI display output.
- `-DLOW_BATTERY_PIN=<num>`: Specifies a GPIO pin that can be polled to get the battery state. By default, when this is set, a low battery icon will be displayed if the pin is pulled low (see `config.h` for ways in which this can be tweaked).

###### New Build Options
- TODO

In addition to the above CMake directives, there are various defines scattered around the codebase, mostly in [config.h](https://github.com/juj/fbcp-ili9341/blob/master/config.h), that control different runtime options. Edit those directly to further tune the behavior of the program. In particular, after you have finished with the setup, you may want to build with `-DSTATISTICS=0` option in CMake configuration line.

##### Build example

Here is a full example of what to type to build and run, if you have the [Adafruit 2.8" 320x240 TFT w/ Touch screen for Raspberry Pi](https://www.adafruit.com/product/1601) with ILI9341 controller:

```bash
cd ~
sudo apt-get install cmake
git clone https://github.com/The-Next-Guy/fbcp-nexus.git
cd fbcp-nexus
mkdir build
cd build
cmake -DSPI_BUS_CLOCK_DIVISOR=6 -DADAFRUIT_ILI9341_PITFT=ON ..
make -j
sudo ./fbcp-ili9341
```

If the above does not work, try specifying `-DSPI_BUS_CLOCK_DIVISOR=8` or `=10` to make the display run a little slower, or try with `-DUSE_DMA_TRANSFERS=OFF` to troubleshoot if DMA might be the issue. If you are using another display controller than ILI9341, using a much higher value, like 30 or 40 may be needed. When changing CMake options, you can reissue the CMake directive line without having to reclone or recreate the `build` directory. However you may need to manually delete file CMakeCache.txt between changing options to avoid CMake remembering old settings.

If you want to do a full rebuild from scratch, you can `rm -rf build` to delete the build directory and recreate it for a clean rebuild from scratch. There is nothing special about the name or location of this directory, it is just my usual convention. You can also do the build in some other directory relative to the fbcp-ili9341 directory if you please.

### Extra Info

Best way to discuss the driver is to open a GitHub issue. The-Next-Guy will be monitoring any new things that pop up, pull requests, bugs, and fixes. Since this still runs just fine for RetroPie, I think its still worth maintaining. For FAQ, Tuning and more information, please check out the [Original Repo](https://github.com/juj/fbcp-ili9341).
