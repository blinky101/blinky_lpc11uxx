#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias(# f)))

// Forward declarations
extern int main(void);
void ResetISR(void);
WEAK void IntDefaultHandler(void);
WEAK extern void __valid_user_code_checksum();


/* cortex-M0 handlers */
void NMI_Handler(void) ALIAS(IntDefaultHandler);
void HardFault_Handler(void) ALIAS(IntDefaultHandler);
void SVC_Handler(void) ALIAS(IntDefaultHandler);
void PendSV_Handler(void) ALIAS(IntDefaultHandler);
void SysTick_Handler(void) ALIAS(IntDefaultHandler);
void FLEX_INT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT2_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT3_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT4_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT5_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT6_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT7_IRQHandler (void) ALIAS(IntDefaultHandler);
void GINT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void GINT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP1_IRQHandler (void) ALIAS(IntDefaultHandler);
void I2C_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP0_IRQHandler (void) ALIAS(IntDefaultHandler);
void UART_IRQHandler (void) ALIAS(IntDefaultHandler);
void USB_IRQHandler (void) ALIAS(IntDefaultHandler);
void USB_FIQHandler (void) ALIAS(IntDefaultHandler);
void ADC_IRQHandler (void) ALIAS(IntDefaultHandler);
void WDT_IRQHandler (void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler (void) ALIAS(IntDefaultHandler);
void FMC_IRQHandler (void) ALIAS(IntDefaultHandler);
void USBWakeup_IRQHandler (void) ALIAS(IntDefaultHandler);


extern void _vStackTop(void);

extern void(*const g_pfnVectors[]) (void);
__attribute__ ((section(".isr_vector")))
void(*const g_pfnVectors[]) (void) = {
    // Core Level - CM0
    &_vStackTop,                    // The initial stack pointer
    ResetISR,						// The reset handler
    NMI_Handler,                    // The NMI handler
    HardFault_Handler,              // The hard fault handler
    0,                              // Reserved
    0,                      	    // Reserved
    0,                              // Reserved
    __valid_user_code_checksum,     // Flash checksum (see linker script)
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    SVC_Handler,                    // SVCall handler
    0,                              // Reserved
    0,                              // Reserved
    PendSV_Handler,                 // The PendSV handler
    SysTick_Handler,                // The SysTick handler

    // LPC11U specific handlers
    FLEX_INT0_IRQHandler,             //  0 - GPIO pin interrupt 0
    FLEX_INT1_IRQHandler,             //  1 - GPIO pin interrupt 1
    FLEX_INT2_IRQHandler,             //  2 - GPIO pin interrupt 2
    FLEX_INT3_IRQHandler,             //  3 - GPIO pin interrupt 3
    FLEX_INT4_IRQHandler,             //  4 - GPIO pin interrupt 4
    FLEX_INT5_IRQHandler,             //  5 - GPIO pin interrupt 5
    FLEX_INT6_IRQHandler,             //  6 - GPIO pin interrupt 6
    FLEX_INT7_IRQHandler,             //  7 - GPIO pin interrupt 7
    GINT0_IRQHandler,                 //  8 - GPIO GROUP0 interrupt
    GINT1_IRQHandler,                 //  9 - GPIO GROUP1 interrupt
    0,                                // 10 - Reserved
    0,                                // 11 - Reserved
    0,                                // 12 - Reserved
    0,                                // 13 - Reserved
    SSP1_IRQHandler,                  // 14 - SPI/SSP1 Interrupt
    I2C_IRQHandler,                   // 15 - I2C0
    TIMER16_0_IRQHandler,             // 16 - CT16B0 (16-bit Timer 0)
    TIMER16_1_IRQHandler,             // 17 - CT16B1 (16-bit Timer 1)
    TIMER32_0_IRQHandler,             // 18 - CT32B0 (32-bit Timer 0)
    TIMER32_1_IRQHandler,             // 19 - CT32B1 (32-bit Timer 1)
    SSP0_IRQHandler,                  // 20 - SPI/SSP0 Interrupt
    UART_IRQHandler,                  // 21 - UART0
    USB_IRQHandler,                   // 22 - USB IRQ
    USB_FIQHandler,                   // 23 - USB FIQ
    ADC_IRQHandler,                   // 24 - ADC (A/D Converter)
    WDT_IRQHandler,                   // 25 - WDT (Watchdog Timer)
    BOD_IRQHandler,                   // 26 - BOD (Brownout Detect)
    FMC_IRQHandler,                   // 27 - IP2111 Flash Memory Controller
    0,                                // 28 - Reserved
    0,                                // 29 - Reserved
    USBWakeup_IRQHandler,             // 30 - USB wake-up interrupt
    0,                                // 31 - Reserved
};



__attribute__ ((section(".after_vectors")))
void data_init(unsigned int romstart, unsigned int start, unsigned int len)
{
    unsigned int *pulDest = (unsigned int *) start;
    unsigned int *pulSrc = (unsigned int *) romstart;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors")))
void bss_init(unsigned int start, unsigned int len)
{
    unsigned int *pulDest = (unsigned int *) start;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = 0;
}

extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

__attribute__ ((section(".after_vectors")))
void ResetISR(void)
{

    unsigned int LoadAddr, ExeAddr, SectionLen;
    unsigned int *SectionTableAddr;

    // Load base address of Global Section Table
    SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
    while (SectionTableAddr < &__data_section_table_end) {
        LoadAddr = *SectionTableAddr++;
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        data_init(LoadAddr, ExeAddr, SectionLen);
    }
    // At this point, SectionTableAddr = &__bss_section_table;
    // Zero fill the bss segment
    while (SectionTableAddr < &__bss_section_table_end) {
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        bss_init(ExeAddr, SectionLen);
    }

    main();
    while(1);
}

__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void)
{
    while (1) {}
}