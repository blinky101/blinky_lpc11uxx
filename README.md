# LPC11uxx Example Project: Blinky

Simple Hello World-like blinky programs for the LPC11uxx platform.
This repositories contains three blinky programs, in increasing order of complexity / abstraction level:

1. [bare-metal](/bare-metal/): A blinky program in one file, with no dependencies at all
2. [basic](/basic/): Some of the low-level stuff is in separate functions, but still a very simple c program
3. [cpm](/cpm/): Using CPM as c package manager: reuseable dependencies are in separate repositories
