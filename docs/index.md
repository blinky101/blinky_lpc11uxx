# LPC11uxx series

<img src="img/lpc11uxx.jpg" alt="LPC11uxx microcontroller chip" width="200">

The LPC11uxx microcontrollers are entry-level 32 bit cortex-m0 microcontrollers with integrated support for USB. A 32-bit cortex-M0 clocked at up to 50MHz is a nice step up from the traditional 8-bit microcontrollers, but as an entry-level chip it is still very accessible for beginners: it is low cost and there are not too many peripherals to setup.

The purpose of this guide is to document the steps required to get started on this platform from scratch, using just a few open-source tools.

## What do you need

* A PCB with an LPC11uxx microcontroller, an LED and a JTAG/SWD connector. You can build one yourself, but if you want to get started quickly, we recommend getting a development board such as the [LPCXpresso board](https://www.nxp.com/support/developer-resources/software-development-tools/lpc-developer-resources-/lpc-microcontroller-utilities/lpcxpresso-board-for-lpc11u24:OM13066) https://www.nxp.com/support/developer-resources/software-development-tools/lpc-developer-resources-/lpc-microcontroller-utilities/lpcxpresso-board-for-lpc11u24:OM13066 <img src="img/lpcxpresso_board.jpg" alt="LPC11uxx microcontroller chip">


* Debugging hardware to get your code on the microcontroller. We advise the [Black Magic Probe](https://github.com/blacksphere/blackmagic/wiki), as it is easy to work with and requires no software dependencies (just gdb). The Black Magic Probe itself is open source, sou you could build one yourself if you wanted to.

* A computer with some free USB ports. This guide assumes you are running linux or MacOS.

* [Gnu Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads): this is a set of open-source tools including the gcc compiler and gdb debugger, specialized for embedded Arm cortex processors.

## Getting started: bare-metal

This repository contains three different blinky projects. The bare-metal project has the least dependencies and is very easy to get up and running. The downside is that the source code is a bit cryptic, especially if you are not used to embedded c code.

### Compile the firmware
```
git clone https://github.com/blinky101/blinky_lpc11uxx.git
cd blinky_lpc11uxx/bare-metal
make all
```

Or in case you don't want to compile the project or it doesn't work for you, you can download the [pre-compiled binary](/bare-metal/blinky.bin) and [elf file](/bare-metal/blinky.elf) directly.

### Flash it using a Black Magic Probe

The Black Magic probe acts as a remote gdb target, so we can flash the firmware directly using gdb.

Remark: you need to install the [Gnu Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads). If the `arm-none-eabi-gdb` command is not available, you need to add the toolchain to your PATH. For example, add this line to ~/.bashrc (assuming linux):
```
export PATH=$PATH:/foler/where/you/installed/the/toolchain/gcc-arm-none-eabi-7-2017-q4-major/bin/
```

#### Connecting the Black Magic Probe

Depending on your platform, a 'file' in /dev/ is created when you plugin the Black Magic Probe. On linux this is most probably something like `/dev/ttyACM0`, on a Mac it is something like `/dev/cu.usbmodem<serialnumber>`.

The firmware can be flashed using this command, replacing /dev/BmpGdb with the correct file as explained above:
```
arm-none-eabi-gdb -nx --batch \
-ex 'target extended-remote /dev/BmpGdb' \
-ex 'monitor swdp_scan' \
-ex 'attach 1' \
-ex 'load' \
-ex 'set mem inaccessible-by-default off' \
-ex 'set {int}0x40048000 = 2' \
-ex 'compare-sections' \
-ex 'kill' \
blinky.elf
```

