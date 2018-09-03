
// called when the chip resets
void reset_vector(void)
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
    .reset = reset_vector,
    .checksum =  (unsigned int)&__valid_user_code_checksum,
};

