# Tutorial blinky lpc11uxx
## From scratch, step by step

This tutorials shows you how to create a blinky project for the lpc11u14 microcontroller without any software dependencies.

---

While just flashing an existing project to your board might be enough te get started, if you really want to understand what is happening, you should start from scratch and build it step by step. That is exactly what this tutorial is about.

In this tutorial we will answer the following questions:

...todo

* Vector table
* Linker File
* reset handler/ reset_isr
* peripheral registers for gpio
* ..

...

### Step 0 Requirements

* GCC embedded toolchain: arm-none-eabi
* black magic debugger probe

See [link] for installation instructions

### Step 1 Create your project

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

### Step 2 Writing the code

Remember, our goal, and our only goal is to blink the LED on pin 0_7. Toggeling a pin is done through the GPIO perihperal. But first we need to make sure that the pin is available for the GPIO peripheral. This is usually called pin multiplexing. The more complex a microcontroller is, the more functions are available per pin.

#### Pin multiplexing

We will need information from the [manual (UM10462)](https://www.nxp.com/docs/en/user-guide/UM10462.pdf). In `Chapter 7 I/O Configuration` we can find the relevant information. More specifically section `7.4.1.8 PIO0_7 register` shows the exact settings for our pin.
The first 3 bits `2:0 FUNC` determine the selected funtion of the pin. There are three options of which only two are valid for parts other than `LPC11U37H`:

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

#### GPIO configuration

Now that the pin belongs to the GPIO peripheral, we can configure it as an output pin.

This is done using the `GPIO port direction registers` located at address `0x5000 2000` (see `chapter 9.5.3.3` of the manual). This 32-bit register defines a `DIR`-bit for every pin in GPIO Port 0. So, bit 0 corresponds with `PIO0_0`, bit 1 with `PIO0_1`, etc. Thus we need to set bit 7 to control the direction of `PIO0_7`. Our second line of code is

```
// configure GPIO direction
(*(volatile unsigned int *)(0x50002000)) |= (1 << 7);
```

#### Toggling the pin high and low

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

