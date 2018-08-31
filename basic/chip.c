#include "chip.h"


void chip_gpio_toggle(uint8_t port, uint8_t pin)
{
    LPC_GPIO->NOT[port] |= (1 << pin);
}

void chip_gpio_set(uint8_t port, uint8_t pin)
{
    LPC_GPIO->SET[port] |= (1 << pin);
}

void chip_gpio_clear(uint8_t port, uint8_t pin)
{
    LPC_GPIO->CLR[port] |= (1 << pin);
}

void chip_gpio_dir_set(uint8_t port, uint8_t pin, uint8_t dir)
{
    LPC_GPIO->DIR[port] |= (dir << pin);
}
