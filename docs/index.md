# LPC11uxx series

<img src="img/lpc11uxx.jpg" alt="LPC11uxx microcontroller chip" width="200">

The LPC11uxx microcontrollers are entry-level 32 bit cortex-m0 microcontrollers with integrated support for USB. A 32-bit cortex-M0 clocked at up to 50MHz is a nice step up from the traditional 8-bit and 16-bit microcontrollers, but as an entry-level chip it is still very accessible for beginners: it is low cost and there are not too many peripherals to setup.

The purpose of this guide is to document the steps required to get started on this platform from scratch, using just a few open-source tools.

## What do you need

* A PCB with an LPC11uxx microcontroller, an LED and a JTAG/SWD connector. You can build one yourself, but if you want to get started quickly, we recommend getting a development board such as the [LPCXpresso board](https://www.nxp.com/support/developer-resources/software-development-tools/lpc-developer-resources-/lpc-microcontroller-utilities/lpcxpresso-board-for-lpc11u24:OM13066) <img src="img/lpcxpresso_board.jpg" alt="LPC11uxx microcontroller chip">


* Debugging hardware to get your code on the microcontroller. We advise the [Black Magic Probe](https://github.com/blacksphere/blackmagic/wiki), as it is easy to work with and requires no software dependencies (just gdb). The Black Magic Probe itself is open source, sou you could build one yourself if you wanted to.

* A computer with some free USB ports. This guide assumes you are running linux or MacOS.

* [Gnu Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads): this is a set of open-source tools including the gcc compiler and gdb debugger, specialized for embedded Arm cortex processors.


## Preparation: Setup the Black Magic Probe

### Connecting the wires
<img src="img/lpcxpresso_with_blackmagic.jpg" alt="LPCXpresso board connected to a Black Magic Probe">

The way to connect depends on the board. For now, we assume the LPCXpresso board (see picture). This is a development board with a built-in NXP LPC-Link debugger. We are not going to use that here, so we have to make some changes to work with the Black Magic Probe:

1. This board does not have a standard 10-pin jtag connector, so we connect via a [Olimex ARM-JTAG-20-10 adapter](https://www.olimex.com/Products/ARM/JTAG/ARM-JTAG-20-10/) and some jumper wires.
  We chose the following color coding in the pictures:
  * red: Power (3.3V)
  * yellow: SWDIO
  * blue: SWCLK
  * black: GND
  
2. We need to cut/remove the connection from the LPC11uxx side of the board to the NXP LPC-Link side of the board.
  This is done by cutting or de-soldering the jumper traces along along the green dotted line before soldering the header.

<img src="img/lpcxpresso_connections.jpg" alt="LPCXpresso board connected to a Black Magic Probe">

### Testing via gdb

Depending on your platform, a 'file' in /dev/ is created when you plugin the Black Magic Probe.

The name of this file can change sometimes, but it is relatively easy to find out by plugging the Black Magic Probe in and out and observing the differences.

* Linux: the file will be something like `/dev/ttyACM0`.
  
  Depending on the linux distribution, you may need permissions to use the debugger. On Ubuntu, this can be fixed by adding yourself to the `dialout` group:
  ```
  sudo adduser <your-username> dialout
  ```
* Mac: the file will be formatted like `/dev/cu.usbmodem<serialnumber>`

You can test if the Black Magic Probe correctly detects the board:

```
arm-none-eabi-gdb -nx --batch \
-ex 'target extended-remote /dev/ttyACM0' \
-ex 'monitor swdp_scan'
```
This should result in something like this:
```
Target voltage: 3.3V
Available Targets:
No. Att Driver
 1      LPC11xx
```
If the command above failed, make sure your target board has power. A nice feature of the Black Magic Probe is that it can provide 3.3V to the target board. So if your board is compatible with 3.3V, try this (note the 'monitor tpwr enable' line):
```
arm-none-eabi-gdb -nx --batch \
-ex 'target extended-remote /dev/ttyACM0' \
-ex 'monitor tpwr enable' \
-ex 'monitor swdp_scan'
```


## Getting started: bare-metal

This repository contains three different blinky projects. The bare-metal project has the least dependencies and is the easiest to get up and running. The downside is that the source code is a bit hard to read, especially if you are not used to embedded c code.

### Compile the firmware
```
git clone https://github.com/blinky101/blinky_lpc11uxx.git
cd blinky_lpc11uxx/bare-metal
make all
```

Or in case you don't want to compile the project or it doesn't work for you, you can download the [pre-compiled binary](https://github.com/blinky101/blinky_lpc11uxx/tree/master/bare-metal/blinky.bin) and [elf file](https://github.com/blinky101/blinky_lpc11uxx/tree/master/bare-metal/blinky.elf) directly.

### Flash it using a Black Magic Probe

The Black Magic probe acts as a remote gdb target, so we can flash the firmware directly using gdb.

Remark: you need to install the [Gnu Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads). If the `arm-none-eabi-gdb` command is not available, you need to add the toolchain to your PATH. For example, add this line to ~/.bashrc (assuming linux):
```
export PATH=$PATH:/folder/where/you/installed/the/toolchain/gcc-arm-none-eabi-7-2017-q4-major/bin/
```

The firmware can be flashed using this command, replacing /dev/ttyACM0 with the correct file for your setup:
```
arm-none-eabi-gdb -nx --batch \
-ex 'target extended-remote /dev/ttyACM0' \
-ex 'monitor swdp_scan' \
-ex 'attach 1' \
-ex 'load' \
-ex 'set mem inaccessible-by-default off' \
-ex 'set {int}0x40048000 = 2' \
-ex 'compare-sections' \
-ex 'kill' \
blinky.elf
```
The output will look similar to this:
```
Loading section .text, size 0x184 lma 0x0
Start address 0xd4, load size 388
Transfer rate: 3 KB/sec, 388 bytes/write.
Section .text, range 0x0 -- 0x184: matched.
Kill the program being debugged? (y or n) [answered Y; input not from terminal]

```
If all goes well, you now have succesfully uploaded your first program!
You should now see a blinking LED (assuming your board has an LED attached to pin PI0_7).
<img src="img/lpcxpresso_blink.jpg" alt="A blinking led" width="600">

### Debugging

The Black Magic Probe is not just for flashing the firmware, you can also step through the code:
```
arm-none-eabi-gdb \
-ex 'target extended-remote /dev/ttyACM0' \
-ex 'monitor swdp_scan' \
-ex 'attach 1' \
-ex 'set mem inaccessible-by-default off' \
-ex 'set {int}0x40048000 = 2' \
blinky.elf
```
This should drop you in a gdb console, showing the code that is currently executing. Some helpful commands:
* continue (continues running the program: the LED blinks)
* Ctrl-C (halts the program: the LED stops blinking)
* ... and many more, see the gdb documentation

This can be very helpful to find bugs in your code, or to verify that the program is indeed running (in case your board does not have an LED to blink).

