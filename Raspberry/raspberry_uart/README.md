# Raspberry-UART-Controller
An uinput controller driver for the Raspberry Pi, that interfaces with an Arduino Pro Mini over the native serial port. Also draws overlays with https://github.com/The-Next-Guy/fbcp-nexus/ If something doesn't work, delete the contents of `config.prop` and rerun the software from the terminal. This starts the setup process.

### Changes made
This is a version of the whole thing that is meant to be run with a Arduino Pro mini as the Serial slave device. Instead of using axis data for the dpad, just using buttons made more sense to me. Additionally, at least the button mapping was a bit weird, which I (ohasanov) changed to suit my needs.

A (south), B (east), X (west), Y (north) is used for layout.
There are two buttons named Extra and Options. Extra is supposed to be the hotkey button for Retropie/Retroarch. Options is thought out for the whole overlay thing the original creator of this repo coded in. 

### Hotkeys (changeable under joyCheck in main.cpp):

Options + A + L + R for a sec -> reboot

Options + B + L + R for a sec -> shutdown

Options + X + L + R for a sec -> restart fbcp service

Options + B -> Battery

Options + X -> Volume (with DPAD UP DOWN to change)

Options + Y -> Brightness (with DPAD UP DOWN to change)

### Thanks
Thanks to The-Next-Guy for creating the original repo, check out https://github.com/The-Next-Guy/picotroller

### Compiling
This program requires `libevdev-dev` to be installed, which can be simply installed with
```
sudo apt-get install libevdev-dev
```

It also requires lodepng.h and lodepng.cpp to be dropped in next to the rest of the source. Ensure the SMA IDs in `shared_memory.h` are the same as the ones set in fbcp-nexus. Create a `config.prop` file next to the source, or else it will throw an error. This is also meant to be ran as a privileged app. 

To build the project, you can just run:
```
make
sudo ./joystick_emulator
```
This will take you through the setup, choose the pico monitor as the other one might be broken after my changes to the code. You can write your own too, by extending Monitor in Monitor.h. You will have to modify the main file to add the option.

If you choose the pico monitor, it will it up and strat running right away on `/dev/serial0` but you can change this by changing the `interface` property.

Once the monitor is setup, it will ask you to setup the buttons, depending on the features provided by the monitor.

Lastly, it will ask you to setup the audio device that it will be using, just enter it like it is. It could be `PCM`, `Headphone`, `HDMI`, or something else.

This whole setup can be ran from ssh, without a keyboard, provided that you have your pico or gpio pins wired up. Who knows, maybe you'll make a wifi client.

To make it start on boot, edit `/etc/rc.local` and add
```
sudo /path/to/pictroller/joystick_emulator&
```

or better yet, maybe create a systemd command to run this on startup?



