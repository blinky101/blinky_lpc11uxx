# LPC11uxx Example Project: Blinky

Simple Hello World-like blinky program for the LPC11uxx using CPM chip libraries.

This projects assumes our specific breakout board for the LPC11uxx chip, however it is easily adapted to different board configurations.

## How To Use

### Prerequisites

- [Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
- [OpenOCD](http://openocd.org)
- CMake

These need to be installed and available in your PATH.

### Build the firmware:

Clone the project, and inside the project folder do:
```
mkdir build
cd build
cmake ..
make
```

### Flash the firware to your board

This assumes you have openocd installed.

run this from the build dir, see build step
```
make flash
```

If everything went right, the firmware should be running and blinking a LED.


## FAQ

### Where are the dependencies? How does this work?

This project uses the CPM package manager, which is basically a few lines of CMake logic.
The CMakeLists.txt contains a list of dependencies, which are automatically checked out.
After building the firmware, all dependencies are found in build/cpm_packages/modules/
