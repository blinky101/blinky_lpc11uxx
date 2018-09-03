
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

