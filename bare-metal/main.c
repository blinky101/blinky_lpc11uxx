
void ResetISR(void);

// see linker script
extern void __valid_user_code_checksum();
extern void _vStackTop(void);

extern void(*const g_pfnVectors[]) (void);
__attribute__ ((section(".isr_vector")))
void(*const g_pfnVectors[]) (void) = {
    // Core Level - CM0
    &_vStackTop,                    // The initial stack pointer
    ResetISR,						// The reset handler
    0,                              // Not used
    0,                              // Not used
    0,                              // Reserved
    0,                      	    // Reserved
    0,                              // Reserved
    __valid_user_code_checksum,     // Flash checksum (see linker script)
};


__attribute__ ((section(".after_vectors")))
void ResetISR(void)
{

    // configure PIO_7 pin function
    (*(volatile unsigned int *)(0x4004401C)) = (2 << 3);
    // configure GPIO direction
    (*(volatile unsigned int *)(0x50002000)) = (1 << 7);

    // set GPIO high
    (*(volatile unsigned int *)(0x50002200)) = (1 << 7);

    while(1) {

        // set LED GPIO low
        (*(volatile unsigned int *)(0x50002280)) = (1 << 7);
        for (int i = 0; i < 100000; ++i) __asm__("nop");

        // set LED GPIO high
        (*(volatile unsigned int *)(0x50002200)) = (1 << 7);
        for (int i = 0; i < 100000; ++i) __asm__("nop");

        // Toggle LED (low)
        (*(volatile unsigned int *)(0x50002300)) = (1 << 7);
        for (int i = 0; i < 300000; ++i) __asm__("nop");

        // Toggle LED (high)
        (*(volatile unsigned int *)(0x50002300)) = (1 << 7);
        for (int i = 0; i < 300000; ++i) __asm__("nop");

    }
}

