#include <stdbool.h>

#ifndef _BOARD_H_
#define _BOARD_H_

/**
 * Configure the required board pins in their correct state
 */
void board_configure_pins(void);

/**
 * Turn board led on of off
 */
void board_led_set(bool on);

/**
 * Toggle the current state of the board LED
 */
void board_led_toggle(void);

#endif
