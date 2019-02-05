#include <stdint.h>

#include <LPC82x.h>

#include <lpc824_bits.h>




static void set_as_input(
const    uint32_t           gpio_bit     ,
volatile uint32_t* const    iocon_reg)
{
    // configure GPIO port as input
    LPC_GPIO_PORT->DIR0 &= ~gpio_bit;
    LPC_GPIO_PORT->CLR0 |=  gpio_bit;

    // enable pulldown resistor
    CHANGE_BITS(*iocon_reg, IOCON_MODES_PULL_MASK, IOCON_MODES_PULL_DOWN);
}



void plex(
const    uint8_t const      *gpio_ndxs ,
volatile uint32_t* const    *iocon_regs,
const    uint8_t             num_gpios )
{
    // enable GPIO
    LPC_SYSCON->SYSAHBCLKCTRL |= SYS_AHB_CLK_CTRL_GPIO_BIT;

    // enable IOCON
    LPC_SYSCON->SYSAHBCLKCTRL |= SYS_AHB_CLK_CTRL_IOCON_BIT;

    // initialize all IO ports as inputs
    for (uint8_t ndx = 0 ; ndx < num_gpios ; ++ndx)
        set_as_input(gpio_ndxs[ndx], iocon_regs[ndx]);
}



uint32_t scan(
const    uint8_t   const        *gpio_ndxs ,
const    uint32_t  const        *gpio_bits ,
volatile uint32_t* const        *iocon_regs,
const    uint8_t                 num_gpios )
{
    uint32_t    keys_pressed = 0;
    uint8_t     out_shift    = 0;

    for (uint8_t out_ndx = 0 ; out_ndx < num_gpios ; ++out_ndx) {
        // configure one IO as output, disable pulldown resistor, output 1
        //
        LPC_GPIO_PORT->DIR0 |= gpio_bits[out_ndx];

        CHANGE_BITS(*iocon_regs[out_ndx],
                    IOCON_MODES_PULL_MASK,
                    IOCON_MODES_PULL_NONE);

        LPC_GPIO_PORT->W0[gpio_ndxs[out_ndx]] = 1;

        // read other IOs
        //
        uint8_t     in_ndx   = 0,
                    in_shift = 0;

        for ( ; in_ndx < out_ndx ; ++in_ndx, ++in_shift)
            if (LPC_GPIO_PORT->W0[gpio_ndxs[in_ndx]])
                keys_pressed |= 1 << out_shift << in_shift;

        ++in_ndx;

        for ( ; in_ndx < num_gpios ; ++in_ndx, ++in_shift)
            if (LPC_GPIO_PORT->W0[gpio_ndxs[in_ndx]])
                keys_pressed |= 1 << out_shift << in_shift;


        // reset output IO back to input
        set_as_input(gpio_bits[out_ndx], iocon_regs[out_ndx]);

        // for next output and set of inputs
        out_shift += num_gpios - 1;
    }

    return keys_pressed;
}
