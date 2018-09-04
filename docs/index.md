# LPC11uxx series

<img src="img/lpc11uxx.jpg" alt="LPC11uxx microcontroller chip" width="200">

The LPC11uxx microcontrollers are entry-level 32 bit cortex-m0 microcontrollers with integrated support for USB. A 32-bit cortex-M0 clocked at up to 50MHz is a nice step up from the traditional 8-bit and 16-bit microcontrollers, but as an entry-level chip it is still very accessible for beginners: it is low cost and there are not too many peripherals to setup.

The purpose of this guide is to document the steps required to get started on this platform from scratch, using just a few open-source tools.

## What do you need

* A PCB with an LPC11uxx microcontroller, an LED and a JTAG/SWD connector. You can build one yourself, but if you want to get started quickly, we recommend getting a development board such as the [LPCXpresso board](https://www.nxp.com/support/developer-resources/software-development-tools/lpc-developer-resources-/lpc-microcontroller-utilities/lpcxpresso-board-for-lpc11u24:OM13066) <img src="img/lpcxpresso_board.jpg" alt="LPC11uxx microcontroller chip">


* Debugging hardware to get your code on the microcontroller. We advise the [Black Magic Probe](https://github.com/blacksphere/blackmagic/wiki), as it is easy to work with and requires no software dependencies (just gdb). The Black Magic Probe itself is open source, sou you could build one yourself if you wanted to.

* A computer with some free USB ports. This guide assumes you are running linux or MacOS.

* [Gnu Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads): this is a set of open-source tools including the gcc compiler and gdb debugger, specialized for embedded Arm cortex processors.


## Tutorials

Build your own blinky project from scratch and understand what is happening with the help of our tutorial.

[Part 0: Getting Started](./getting_started)
[Part 1: bare-metal blinky](./tutorial_part1)
