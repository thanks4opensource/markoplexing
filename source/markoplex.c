#include <stdint.h>

#include <LPC82x.h>

#include <lpc824_bits.h>



static const uint32_t        CMP_LADDER_LADSEL_VREF_BITS = 20 << 1;


// must match colr_GPIO_NDX in main.c
// note is reason why has to be GPIO ports 0, 1, 14 and 23

static const uint32_t        BRWN_SWM_BIT = SWM_PINENABLE0_ACMP_I2_EN_BIT,
                             PRPL_SWM_BIT = SWM_PINENABLE0_ACMP_I4_EN_BIT,
                             BLUE_SWM_BIT = SWM_PINENABLE0_ACMP_I3_EN_BIT,
                             GRAY_SWM_BIT = SWM_PINENABLE0_ACMP_I1_EN_BIT;

static const uint32_t        BRWN_CMP_BIT = CMP_CTRL_COMP_VP_SEL_ACMP_I2_BITS,
                             PRPL_CMP_BIT = CMP_CTRL_COMP_VP_SEL_ACMP_I4_BITS,
                             BLUE_CMP_BIT = CMP_CTRL_COMP_VP_SEL_ACMP_I3_BITS,
                             GRAY_CMP_BIT = CMP_CTRL_COMP_VP_SEL_ACMP_I1_BITS;

static const uint32_t        SWM_BITS[] = {BRWN_SWM_BIT,
                                           PRPL_SWM_BIT,
                                           BLUE_SWM_BIT,
                                           GRAY_SWM_BIT},
                             CMP_BITS[] = {BRWN_CMP_BIT,
                                           PRPL_CMP_BIT,
                                           BLUE_CMP_BIT,
                                           GRAY_CMP_BIT};



void plex(
const    uint8_t const      *gpio_ndxs ,
volatile uint32_t* const    *iocon_regs,
const    uint8_t             num_gpios )
{
    // enable GPIO
    LPC_SYSCON->SYSAHBCLKCTRL |= SYS_AHB_CLK_CTRL_GPIO_BIT;

    // enable IOCON
    LPC_SYSCON->SYSAHBCLKCTRL |= SYS_AHB_CLK_CTRL_IOCON_BIT;

    // disable pulldown resistors
    for (uint8_t ndx = 0 ; ndx < num_gpios ; ++ndx)
        CHANGE_BITS(*iocon_regs[ndx],
                    IOCON_MODES_PULL_MASK,
                    IOCON_MODES_PULL_NONE);

    // initialize analog comparator
    //
    LPC_SYSCON->SYSAHBCLKCTRL |= SYS_AHB_CLK_CTRL_ACMP_BIT;

    // power on by clearing bit
    LPC_SYSCON->PDRUNCFG &= ~SYS_CON_PDRUNCFG_ACMP_PD_BIT;

    LPC_CMP->LAD =   CMP_LADDER_LADEN_BIT
                   | CMP_LADDER_LADSEL_VREF_BITS
                   | CMP_LADDER_LADREF_VDD_BIT  ;

    // VP_SEL_LADDER_BITS temporary: will be reset to each comparator input pin
    LPC_CMP->CTRL  =   CMP_CTRL_COMP_VM_SEL_LADDER_BITS
                     | CMP_CTRL_COMP_VM_SEL_LADDER_BITS;

    // set and clear to clear the COMPEDGE bit
    LPC_CMP->CTRL |=  CMP_CTRL_EDGECLR_BIT;
    LPC_CMP->CTRL &= ~CMP_CTRL_EDGECLR_BIT;

    // initialize switch matrix and all IO ports as analog inputs
    //

    LPC_SYSCON->SYSAHBCLKCTRL |= SYS_AHB_CLK_CTRL_SWM_BIT;

    // enable analog comparators
    for (uint8_t ndx = 0 ; ndx < num_gpios ; ++ndx)
        LPC_SWM->PINENABLE0 &= ~SWM_BITS[ndx];

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
        // configure one IO as normal GPIO output, output Vdd
        //
        LPC_SWM->PINENABLE0    |= SWM_BITS [out_ndx];     // ~ACMP
        LPC_GPIO_PORT->DIRSET0 |= gpio_bits[out_ndx];     // set as output

        LPC_GPIO_PORT->W0[gpio_ndxs[out_ndx]] = 1;        // output 1

        // read other IOs
        //
        uint8_t in_shift = 0;

        for (uint8_t in_ndx = 0 ; in_ndx < num_gpios ; ++in_ndx) {
            if (in_ndx == out_ndx) continue;

            CHANGE_BITS(LPC_CMP->CTRL            ,
                        CMP_CTRL_COMP_VP_SEL_MASK,
                        CMP_BITS[in_ndx]         );


            if (LPC_CMP->CTRL & CMP_CTRL_COMPSTAT_BIT)
                keys_pressed |= 1 << out_shift << in_shift;

            ++in_shift;
        }

        // reset output GPIO back to being analog comparator
        LPC_GPIO_PORT->W0[gpio_ndxs[out_ndx]] = 0;       // output 0

        LPC_GPIO_PORT->DIRCLR0  =  gpio_bits[out_ndx];   // input
        LPC_SWM->PINENABLE0    &= ~SWM_BITS [out_ndx];   // ACMP

        // for next output and set of inputs
        out_shift += num_gpios - 1;
    }

    return keys_pressed;
}
