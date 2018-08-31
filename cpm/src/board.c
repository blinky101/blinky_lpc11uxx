#include "board.h"
#include "board_GPIO_ID.h"

#include <lpc_tools/boardconfig.h>
#include <lpc_tools/GPIO_HAL.h>
#include <c_utils/static_assert.h>

#include <chip.h>

// Oscillator frequency, needed by chip libraries
const uint32_t OscRateIn = 12000000;


typedef struct {
	uint32_t port : 8;			/* Pin port */
	uint32_t pin : 8;			/* Pin number */
	uint32_t modefunc : 16;		/* Function and mode */
} PINMUX_GRP_T;

static const NVICConfig NVIC_config[] = {
    {TIMER_32_0_IRQn,       1},     // delay timer: high priority
};

static const PinMuxConfig pinmuxing[] = {

        // TODO FIXME IOCON -> more generic name
        // Board LEDs 
        {0,  7, (IOCON_FUNC0)},          // LED
};

static const GPIOConfig pin_config[] = {
    {{0,  7}, GPIO_CFG_DIR_OUTPUT_LOW},     // GPIO_ID_LED
};

// pin config struct should match GPIO_ID enum
STATIC_ASSERT( (GPIO_ID_MAX == (sizeof(pin_config)/sizeof(GPIOConfig))));


static const BoardConfig config = {
    .nvic_configs = NVIC_config,
    .nvic_count = sizeof(NVIC_config) / sizeof(NVIC_config[0]),

    .pinmux_configs = pinmuxing,
    .pinmux_count = sizeof(pinmuxing) / sizeof(pinmuxing[0]),
    
    .GPIO_configs = pin_config,
    .GPIO_count = sizeof(pin_config) / sizeof(pin_config[0]),

    .ADC_configs = NULL,
    .ADC_count = 0
};


void board_setup(void)
{
    board_set_config(&config);
}

