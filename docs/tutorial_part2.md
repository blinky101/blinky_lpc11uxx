---
title: Tutorial Part 2
---

# Tutorial Part 2

The previous tutorial showed how to get a bare-metal blinky up and running.
In this tutorial, we will expand the bare-metal example, adding some abstractions to make the main program a bit easier to work with.

> **Note: If you have skipped [part 1](./tutorial_part1), you can [download the code from here](https://github.com/blinky101/blinky_lpc11uxx/tree/master/bare-metal/).**

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
 This tells the linker that all data (that is, global variables that are explicitly initialized) will be in RAM, but must also be in flash memory. The idea is that the variables from flash must be copied to RAM before the program starts. Try and run the program again: it should now work.
 
 While adding that section to the linker script appears to fix the program, we still have a missing link: how are the global variables in RAM initialized? The answer is that we need to do this ourselves at the begin of our program. We don't do that yet, which can be demonstrated by the following changes:
 
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

## Getting globals to work: Linker Script

You could say there are three kinds of global variables in c:
* constants: they are stored in flash and can never change.
    Example:
    ```
    const int foo = 3;
    ````
* data: They are in RAM, but must be initialized from flash.
    Example:
    ```
    int foo = 3; // may be changed later by the program
    ```
* bss: they are in RAM, but we should initialze them to zero.
    Example:
    ```
    int foo; // according to the c standard, should be 0
    ```

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

## Getting globals to work: Startup Code

Because setting up the data section is one of the first things to do at startup, we will call it the *startup code*.
To initialize the data section, we write a simple function to copy data from flash to RAM (The `i+=4` part is because len will be supplied in bytes, while the pointers are 32-bit = 4 bytes in size):

```
void data_init(unsigned int romstart, unsigned int start, unsigned int len)
{
    unsigned int *dst = (unsigned int *) start;
    unsigned int *src = (unsigned int *) romstart;
    unsigned int i;
    for (i = 0; i < len; i+=4) {
        *dst++ = *src++;
    }
}
```

We have to keep it simple: the startup code cannot rely on features that need the startup code to be finished! The function to initialize bss is even simpler:
```
void bss_init(unsigned int start, unsigned int len)
{
    unsigned int *dst = (unsigned int *) start;
    unsigned int i;
    for (i = 0; i < len; i+=4) {
        *dst++ = 0;
    }
}
```

To call these functions, we first need access to the *Data Section Table* from the linker script. Add the following lines to the top of main.c:
```
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;
```

Then we add the logic to call data_init and bss_init:
```
void prepare_startup(void)
{
    unsigned int load_addr, exe_addr, section_len;
    unsigned int *section_table_addr;

    // Load base address of Global Section Table
    section_table_addr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
    while (section_table_addr < &__data_section_table_end) {
        load_addr = *section_table_addr++;
        exe_addr = *section_table_addr++;
        section_len = *section_table_addr++;
        data_init(load_addr, exe_addr, section_len);
    }
    // At this point, section_table_addr = &__bss_section_table;
    // Zero fill the bss segment
    while (section_table_addr < &__bss_section_table_end) {
        exe_addr = *section_table_addr++;
        section_len = *section_table_addr++;
        bss_init(exe_addr, section_len);
    }
}
```

When you call `prepare_startup()` at the beginning of the program, the code should work. Try experimenting by changing the program, adding global variabes etc.

## Refactoring: startup code

Hopefully this was a good demonstration of what the startup code does and why you need it. While it is good to know how it works, you almost never need to change it and will share it across many projects. A nice way to refactor this is to put all the startup-related code in a separate file. In this case, we could rename `main.c` to `startup.c` and add an empty `main.c`, containing just this:
```
int main()
{
    while(1);
}
```
Then move the blinking logic to `int main()` and call `main()` from the end of the startup code.
Now we have clearly divided the program in two parts:
* `startup.c` manages the low-level details of setting up a valid c runtime environment
* `main.c` contains the usual main() function: this code can use the normal c features that you expect.

## Refactoring: chip library

Now that we finaly have a sane c environment, a good next step would be to refactor the blinky logic itself. After all, it is not very fun to have to remember the memory addresses of every peripheral. There are so many peripherals and features on even a simple microcontroller like this, so writing a header file would be a lot of work. Fortunately, these files are already available.

This already simplifies toggling a LED:
```diff
- (*(volatile unsigned int *)(0x50002300)) = (1 << 7);
+ LPC_GPIO->NOT[0] |= (1 << 7)
```

On top of that, you could write some wrapper functions / macros to make it even nicer:
```diff
- LPC_GPIO->NOT[0] |= (1 << 7)
+ chip_gpio_toggle(0,7);
```
Have a look at [the 'basic' project from the repository](https://github.com/blinky101/blinky_lpc11uxx/tree/master/basic/) to see the end result: main.c contains just the blinky logic, startup.c contains all the startup stuff and the other files form a very basic chip library.

<div class="tutorial_nav">
  <a class="left" href="./tutorial_part1">◀ Tutorial Part 1</a>
  <a class="right" href="./tutorial_part3">Tutorial Part 3 ▶</a>
</div>
