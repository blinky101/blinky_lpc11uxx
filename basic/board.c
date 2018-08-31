#include "board.h"
#include "chip.h"

void board_configure_pins(void) {

    // configure GPIO direction for led pin
    chip_gpio_dir_set(0, 7, 1);
}

void board_led_set(bool on) {
    if (on) {
        chip_gpio_set(0, 7);
    } else {
        chip_gpio_clear(0, 7);
    }
}

void board_led_toggle(void) {
    chip_gpio_toggle(0, 7);
}