CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
CPUFLAGS = -mcpu=cortex-m0 -mthumb
CFLAGS = -Wall -Wextra -g3 -O0 -MD $(CPUFLAGS)
LDFLAGS = $(CPUFLAGS) -nostartfiles -Wl,-T,link.ld
LDLIBS = -lc -lnosys

CSRC = main.c startup.c board.c chip.c
OBJ = $(CSRC:.c=.o)

blinky.elf: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

.PHONY: clean
clean:
	-rm -rf *.o *.bin *.elf *.map -include *.d

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

all: blinky.bin
