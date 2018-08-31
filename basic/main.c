#include "board.h"

int main() {

    board_configure_pins();


    while(1) {

        // set LED GPIO low
        board_led_set(0);
        for (int i = 0; i < 100000; ++i) __asm__("nop");

        // set LED GPIO high
        board_led_set(1);
        for (int i = 0; i < 100000; ++i) __asm__("nop");

        // Toggle LED (low)
        board_led_toggle();
        for (int i = 0; i < 300000; ++i) __asm__("nop");

        // Toggle LED (high)
        board_led_toggle();
        for (int i = 0; i < 300000; ++i) __asm__("nop");

    }

    return 0;

}