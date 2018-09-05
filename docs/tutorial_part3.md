---
title: Tutorial Part 3
---

# Tutorial Part 3: CPM

The [previous tutorial](./tutorial_part_2) showed how to get to a working c program and introduced some nice abstractions.
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
* **config.cmake** : a config file with some local settings (e.g. optimization level, location of the Black Magic Probe

> **TODO: this tutorial is under construction!**
