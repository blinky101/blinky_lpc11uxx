#include <stdint.h>
#include "LPC11Uxx.h"

#ifndef _CHIP_H_
#define _CHIP_H_

/**
 * Configure the direction of the gpio pin
 * @param port gpio port [0, 1]
 * @param pin gpio pin within the port
 * @param dir the gpio direction, 0 = input, 1 = output
 */
void chip_gpio_dir_set(uint8_t port, uint8_t pin, uint8_t dir);

/**
 * Toggles the current state of gpio `pin` at `port`
 */
void chip_gpio_toggle(uint8_t port, uint8_t pin);

/**
 * Set the pio pin high
 */
void chip_gpio_set(uint8_t port, uint8_t pin);

/**
 * Pull the pio pin low
 */
void chip_gpio_clear(uint8_t port, uint8_t pin);

#endif