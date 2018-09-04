---
title: Tutorial Part 2
---

# Tutorial Part 2

The previous tutorial showed how to get a bare-metal blinky up and running.
In this tutorial, we will expand the bare-metal example, adding some abstractions to make the main program a bit easier to work with.

## This is not a c program (yet)

[The end result from part 1](https://github.com/blinky101/blinky_lpc11uxx/tree/master/bare-metal/) of the tutorial almost looks like a normal c program, but there are still some bits missing.
As an example, change the `blinky()` function to this:

```
int blink_time;

void blinky(void)
{

    // configure PIO_7 pin function
    (*(volatile unsigned int *)(0x4004401C)) = 0;

    // configure GPIO direction
    (*(volatile unsigned int *)(0x50002000)) |= (1 << 7);

    while(1) {

        blink_time = 100000;

        // set LED GPIO low
        (*(volatile unsigned int *)(0x50002280)) = (1 << 7);
        for (int i = 0; i < blink_time; ++i) __asm__("nop");

        // set LED GPIO high
        (*(volatile unsigned int *)(0x50002200)) = (1 << 7);
        for (int i = 0; i < blink_time; ++i) __asm__("nop");
    }
}
```
This is very similar to the previous version, but instead of hardcoded values, we now use a global variable to control the delay time. You would expect this program to run fine, but it does not. Why?

C is considered a low-level language by most people, but even c needs some special setup to be completed before it can run properly. For example, think about the global variable we declared. Where should it be stored in memory? This must be specified in the linker file:

```
/* Main DATA section (RAM) */
	.data : ALIGN(4)
	{
	   FILL(0xff)
	   _data = . ;
	   *(.data*)
	   . = ALIGN(4) ;
	   _edata = . ;
	} > RAM AT>Flash
  ```
 This tells the compiler that all data (that is, global variables that are explicitly initialized) will be in RAM, but must also be in flash memory. The idea is that the variables from flash must be copied to RAM before the program starts. Try and run the program again: it should now work.
 
 While adding that section to the linker script appears to fix the program, but we still have a missing link: how are the global variables in RAM initialized? The answer is that we need to do this ourselves at the begin of our program. We don't do that yet, which can be demonstrated by the following changes:
 
 1. initialize blink_time to a high value (5 milion in this example)
 ```
 int blink_time = 5000000;
 ```
 2. change to a faster blink time after a few blinks (100k in this example)
 ```
 int count=0;
 while(1) {
    if(count++ > 10) {
        blink_time = 100000;
    }
```
Normally, this code should result in a (very) slow blinking led, which starts to blink a lot faster after 10 iterations. But we never copy the global variable data from flash to RAM: this means the global is never initialized, and the first 10 iterations are not at the expected speed.

## Getting globals to work

You could say there are three kinds of global variables in c:
* constants: they are stored in flash and can never change.
    Example: `const int foo = 3;`
* data: They are in RAM, but must be initialized from flash.
    Example: `int foo = 3; // may be changed later by the program`
* bss: they are in RAM, but we should initialze them to zero.
    Example: `int foo; // according to the c standard, should be 0`

To get `.data` and `.bss` to work, we must first ensure they are in the linker script. Change the linker script so it includes these sections:
```
/* Main DATA section (RAM) */
.data : ALIGN(4)
{
   FILL(0xff)
   _data = . ;
   *(.data*)
   . = ALIGN(4) ;
   _edata = . ;
} > RAM AT>Flash

/* MAIN BSS SECTION */
.bss : ALIGN(4)
{
    _bss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4) ;
    _ebss = .;
    PROVIDE(end = .);
} > RAM
```
The bss section is very similar to the data section, but these variables are only in RAM: saving a bunch of zeroes in flash would only waste space.

To initialize the `.data` and `.bss` sections, we must know where they are in memory, how large they are and where to get the data. To get this information available in the main program, we will change the `.text` section to include a so-called *Global Section Table*. This contains:
* a data_section_table: a table with one entry: the properties of the `.data` section
* a bss_section_table: another table with one entry: the properties of the `.bss` section

In a more complex setup you could add another entry to these tables to define a custom memory region you want to be initialized or zeroed out before the program starts. The `.text` section including the Section Table:

```
.text : ALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.interrupt_vector_table))

        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

         *(.text*)

    } > Flash
```

## TODO: This tutorial is under construction.
