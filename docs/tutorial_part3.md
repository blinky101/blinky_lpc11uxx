---
title: Tutorial Part 3
---

# Tutorial Part 3: CPM

The [previous tutorial](./tutorial_part2) showed how to get to a working c program and introduced some nice abstractions.
In this tutorial, we will show a way to put common code into separate libraries and re-using them across projects.

A common way to re-use code libraries is through a package manager. We are going to use [CPM: the C Package Manager](https://github.com/iauns/cpm). CPM is implemented in CMake, so we will switch from *Make* to *CMake* as our build system.

## CPM packages

The basic principle is this: in the CMake build system, we include a file that implements [CPM:](https://github.com/iauns/cpm). We can then list our dependencies, for example:

```
CPM_AddModule("chip_lpc11uxx"
    GIT_REPOSITORY "https://github.com/JitterCompany/chip_lpc11uxx.git"
    GIT_TAG "1.1")
```
The example above lists the package "chip_lpc11uxx" as a dependency. In this case we specified version `1.1`, but this can be any git tag or simply `master`. Another way to list a dependency would be this:
```
CPM_AddModule("my_library"
    SOURCE_DIR "/local/filesystem/path/to/my_library")
```
This is very usefull during debugging or developing such a library. Otherwise you would constantly need to push your git changes.

A CPM module itself is not very complicated. As an example, take a look at the [chip_lpc11uxx package](https://github.com/JitterCompany/chip_lpc11uxx.git). It contains:

* A CMakeLists.txt file: declares the name of the package, contains a few lines of *CPM* boilerplate and a few build rules.
* a folder *chip_lpc11uxx* (this must match the name of the package) containing the source files

## Project structure

A typical firmware project using the CPM system looks like this:
* **build** : a folder for the build results
* **cmake** : some cmake helper scripts to setup CPM and define the Arm Embedded Toolchain
* **src**   : a folder containing the c source code and headers (*.c and *.h files)
* **link.ld** : the linker script
* **config.cmake** : a config file with some local settings (e.g. optimization level, location of the Black Magic Probe)

Have a look at the [cpm blinky project](https://github.com/blinky101/blinky_lpc11uxx/tree/master/cpm/). It is basically the same code as in [part 2 of the tutorial](./tutorial_part2), but now as a CPM project. This projects has a few dependencies:

* **startup_lpc11uxx**: startup code, as explained before
* **chip_lpc11uxx**: chip library. A more advanced/complete version of the chip level abstractions we introduced before.
* **lpc_tools**: contains some abstractions over the chip library. This makes it easier to port code between lpc11uxx and other lpc-series microcontrollers
* **mcu_timing**: timing-related library. provides functions to delay or count intervals of time (see main.c)
* **c_utils**: a few utility functions. Not really microcontroller-related, just plain C.
* **mcu_debug**: Provides an easy debug and flash experience.

All these packages are open-source, sou you can check them out on github if you want. If you want to create your own package, the easiest way is to model it after a simple existing package, for example *c_utils*.

## Configuration

This project (as most cpm firmware packages) has a separate file where you can customize some settings.
Create the config file as below, then edit it with your favorite text editor:
```
cp config.cmake.example config.cmake
```

## Building

As stated in the README for the cpm project, you can build the firmware like this:
```
mkdir build
cd build
cmake ..
make
```

## Flashing and debugging

Untill now, we had to copy-paste quite long commands to flash the firmware to our board. In the cpm-based project this is much simpler: the *mcu_debug* package adds some nice shortcuts for this.

To flash the firmware:
```
make flash
```
To start debugging with gdb:
```
make debug
```
For more info, check the [README](https://github.com/blinky101/blinky_lpc11uxx/tree/master/cpm).

## The End

This concludes the tutorial series about the lpc11uxx. We started with a [bare-metal](https://github.com/blinky101/blinky_lpc11uxx/tree/master/bare-metal) project from scratch, got to a [basic](https://github.com/blinky101/blinky_lpc11uxx/tree/master/basic) example and finally wrapped it all up in a [cpm package](https://github.com/blinky101/blinky_lpc11uxx/tree/master/cpm).

<div class="tutorial_nav">
  <a class="left" href="./tutorial_part2">◀ Tutorial Part 2</a>
</div>
