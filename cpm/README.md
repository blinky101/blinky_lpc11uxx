# CPM-based blinky project

## How To Use

### Prerequisites

- [Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
- A [Black Magic Probe](https://github.com/blacksphere/blackmagic/wiki) or [OpenOCD](http://openocd.org) in combination with a [JTAG LockPick tiny 2](http://www.distortec.com/jtag-lock-pick-tiny-2/)
- CMake

Make sure all required software is installed correctly and available in your PATH.

### Build the firmware:

Clone the project, and go to the project folder (the folder containing CMakeLists.txt).
Inside this folder, create a build folder and build the firmware:
```
cp config.cmake.example config.cmake
# review the settings in config.cmake

mkdir build
cd build
cmake ..
make
```

### Flash the firware to your board

This step flashes the firmware via either the [Black Magic Probe](https://github.com/blacksphere/blackmagic/wiki) (default) or via connected, or via [OpenOCD](http://openocd.org).

Connect your debugger to the target board, and run the following command from the build dir (see build step above):
```
make flash
```

### Debugging via gdb

This works similarly to flashing.
Connect your debugger to the target board, and run the following command from the build dir (see build step above):
```
make debug
```
This should drop you into a gdb console.
When exiting gdb (e.g. via ctrl-C), you may see some cmake errors/warnings. These can be safely ignored.


## FAQ

### Where are the dependencies? How does this work?

This project uses the CPM package manager, which is basically a few lines of CMake logic.
The CMakeLists.txt contains a list of dependencies, which are automatically checked out.
After building the firmware, all dependencies are found in build/cpm_packages/modules/


### Why does the Black Magic Probe not work? Why is OpenOCD tried instead?

The script automatically tries to connect to the Black Magic Probe. If it cannot be found, it falls back to OpenOCD.
If the firmware tries to flash via OpenOCD, it means that your probe is not detected properly.
You can specify the Black Magic Probe in config.cmake:
```
cp config.cmake.example config.cmake

# edit this line to match your Black Magic Device
set(BLACKMAGIC_DEV /dev/ttyBmpGdb)
```

