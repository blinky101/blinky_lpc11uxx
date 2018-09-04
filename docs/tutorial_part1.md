---
title: Tutorial Part 1
---

# Tutorial blinky lpc11uxx

This tutorials shows you how to create a blinky project for the lpc11u14 microcontroller from scratch without any software dependencies.

In this tutorial we will learn about

* Pin multiplexing
* Peripheral registers for gpio
* Interrupt vector table
* Linker file

## Step 0: Requirements

* GNU Arm Embedded Toolchain
* Black Magic Probe for flashing the firmware

<!-- See [link] for installation instructions -->

## Step 1: Create your project

Create a directory

```
mkdir blinky_lpc11uxx
cd blinky_lpc11uxx
```

To start we will need 2 files. the code will reside in `main.c` and `link.ld` will be needed by the linker for building the binaries.

```
touch main.c link.ld
```
This will create the (empty) files in the required directory.

## Step 2: Writing the code

Remember, our goal, and our only goal is to blink the LED on pin 0_7. Toggeling a pin is done through the GPIO perihperal. But first we need to make sure that the pin is available for the GPIO peripheral. This is usually called pin multiplexing. The more complex a microcontroller is, the more functions are available per pin.

### Pin multiplexing

We will need information from the [manual (UM10462)](https://www.nxp.com/docs/en/user-guide/UM10462.pdf). In `Chapter 7 I/O Configuration` we can find the relevant information. More specifically section `7.4.1.8 PIO0_7 register` shows the exact settings for our pin.
The first 3 bits `2:0 FUNC` determine the selected function of the pin. There are three options of which only two are valid for parts other than `LPC11U37H`:

* `0x0: PIO0_7` - use the pin as a GPIO pin, this is what we need/
* `0x1: C̅T̅S̅` - Clear To Send, which is a UART feature.
* `0x2: IOH_5` - (only available on LPC11U37H)

The other fields of this register we can ignore for now. So that means that we need to write `0b00` to the first 2 bits of the register. The manual shows that the `PIO0_7 register` is located at memory address `0x4004 401C`.
This results in our first line of c code:

```
// configure PIO_7 pin function
(*(volatile unsigned int *)(0x4004401C)) = 0;
```
What this does is, first we cast the address to an *int pointer*: `(volatile unsigned int *)(0x4004401C)`, then we dereference it and write a zero to that place in memory that happens to be a specific register.

*Note that it is not very good practice to write (anything) to reserved register values, since it can have unintended side effects, but in this case it is harmless. The better way is to first read the value, then only set or clear the bits you want to change and then write the value back.*

### GPIO configuration

Now that the pin belongs to the GPIO peripheral, we can configure it as an output pin.

This is done using the `GPIO port direction registers` located at address `0x5000 2000` (see `chapter 9.5.3.3` of the manual). This 32-bit register defines a `DIR`-bit for every pin in GPIO Port 0. So, bit 0 corresponds with `PIO0_0`, bit 1 with `PIO0_1`, etc. Thus we need to set bit 7 to control the direction of `PIO0_7`. Our second line of code is

```
// configure GPIO direction
(*(volatile unsigned int *)(0x50002000)) |= (1 << 7);
```

### Toggling the pin high and low

Now that we did all the preparations we can finally do do the real blinking. We need to toggle the pin between a high state (3.3V) and a low state (0V). There are multiple ways to do this, but for this tutorial we will use the `GPIO port set` and `GPIO port clear` registers. See sections `9.5.3.7` and `9.5.3.8` of the manual. Similarly to the GPIO direction register, we should now write to the 7th bit of the CLEAR and SET registers.

```
// set LED GPIO low
(*(volatile unsigned int *)(0x50002280)) = (1 << 7);


// set LED GPIO high
(*(volatile unsigned int *)(0x50002200)) = (1 << 7);
```

Don't forget to add some delay `for (int i = 0; i < 100000; ++i) __asm__("nop");`so that our slow human eyes can actually see the blinking. The delay is just a for loop that does noting (`__asm__("nop")` is the assembly nop operator, which means no-operation) during 100000 loops

And finally we wrap it in a `while(1)` loop so that it will go on forever:

```
while(1) {

    // set LED GPIO low
    (*(volatile unsigned int *)(0x50002280)) = (1 << 7);
    for (int i = 0; i < 100000; ++i) __asm__("nop");

    // set LED GPIO high
    (*(volatile unsigned int *)(0x50002200)) = (1 << 7);
    for (int i = 0; i < 100000; ++i) __asm__("nop");
}
```

Putting all our code inside a function `blinky()` in the main.c file:

```
void blinky(void)
{

    // configure PIO_7 pin function
    (*(volatile unsigned int *)(0x4004401C)) = 0;

    // configure GPIO direction
    (*(volatile unsigned int *)(0x50002000)) |= (1 << 7);

    while(1) {

        // set LED GPIO low
        (*(volatile unsigned int *)(0x50002280)) = (1 << 7);
        for (int i = 0; i < 100000; ++i) __asm__("nop");

        // set LED GPIO high
        (*(volatile unsigned int *)(0x50002200)) = (1 << 7);
        for (int i = 0; i < 100000; ++i) __asm__("nop");

    }
}
```

Now we can try to compile it with the following command:

```
arm-none-eabi-gcc -Wall -Wextra -g3 -O0 -MD -mcpu=cortex-m0 -mthumb -c -o main.o main.c
```
If everything went OK we didn't get any warning or errors and now we have a `main.o` object file in our directory. Unfortunately, we're not quite ready to run this code on our hardware.

We still need to tell the compiler and our microcontroller where to place the code and where to find it. That's where the `link.ld` file we created earlier comes into play.

## Step 3: Linker File and Interrupt Vector Table

[General intro to linkerfile?]

The linker file basically does two things. Define the available memory, and define what should be put into it.

For this project the minimum viable linker file is as follows

```

MEMORY
{
  /* Define each memory region */
  Flash (rx) : ORIGIN = 0x0, LENGTH = 0x8000 /* 32K bytes */
  RAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x0800 /* 2K bytes */
}

/* Define a symbol for the top of each memory region */
__top_RAM = 0x10000000 + 0x2000;

/* reset_vector is the entry point of the program */
ENTRY(blinky)

SECTIONS
{
    .text : ALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.interrupt_vector_table))
         *(.text*)

    } > Flash


    /* Pointer to top of the stack */
    PROVIDE(_vStackTop = __top_RAM - 0);

    /* Calculate the usercode checksum as per the LPC11uxx user manual:
     * UM10452, chapter 20.7.
     */
    PROVIDE(__valid_user_code_checksum = 0 -
        (_vStackTop
        + (blinky + 1)
        + (0)
        + (0)
        + (0)
        + (0)
        + (0)
        )
    );
}

```
We can just copy this to `link.ld`.

#### Memory section

The linker file starts with a memory section. This section defines all the memory available, which for the LPC11uxx is Flash and RAM. If we take a look at the memory map in chapter 2.2 of the manual, we see that there is on-chip flash starting at address `0x0000 0000` which has a size of 32 kB on the LPC11u14. Additionally, there is 2kB or RAM located at address `0x1000 0000`.

```
MEMORY
{
  /* Define each memory region */
  Flash (rx) : ORIGIN = 0x0, LENGTH = 0x8000 /* 32K bytes */
  RAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x0800 /* 2K bytes */
}
```

#### Sections section

The rest of the linker file, which consists mostly of the `SECTIONS` section, tells the compiler where the code needs to be and where the Interrupt vector table should be.

#### Interrupt Vector Table

Finally we need to add this code to `main.c`. The special compiler attribute `__attribute__ ((section(".interrupt_vector_table")))` makes sure that the following `interrupt_vector_table` struct will be placed at the correct memory address as defined in the linker file.
This table is defined by the Cortex-M0 architecture. It will always start with the stack pointer followed by the reset interrupt vector. We have pointed the `.reset` to our blinky function so that the microcontroller will start code execution there.


```
// these symbols are defined in the linker script
extern unsigned int __valid_user_code_checksum;
extern unsigned int _vStackTop;

// setup the interrupt vector table
__attribute__ ((section(".interrupt_vector_table")))
struct {
    void *stack;
    void (*reset)(void);
    void *_unused[5];
    unsigned int checksum;
    void *__unused[40];
} interrupt_vector_table = {

    .stack = &_vStackTop,
    .reset = blinky,
    .checksum =  (unsigned int)&__valid_user_code_checksum,
};
```

#### Checksum

The checksum is a special LPC feature which is used by the onboard LPC bootloader to determine whether there valid code exists in flash. The linker file calculates the correct value and the interrupt vector table makes sure it is placed at the correct location. See chapter `20.7` of the user manual.


# Compile and link and flash

Now we have everything to run our code. Issue the following commands to compile and link our code respectively.

```
arm-none-eabi-gcc -Wall -Wextra -g3 -O0 -MD -mcpu=cortex-m0 -mthumb -c -o main.o main.c
arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -nostartfiles -Wl,-T,link.ld -o blinky.elf main.o -lc -lnosys
```

After this we should have a `blinky.elf` file. We can flash our code to the board using the Black Magic Probe:

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

<!--  See [TODO] for more detailed information about flashing your target board. -->


