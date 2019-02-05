#include <stdint.h>

#include <LPC82x.h>

#include <lpc824_bits.h>


#define SPI             LPC_SPI0
static const uint32_t   SYS_AHB_CLK_CTRL_SPI_BIT = SYS_AHB_CLK_CTRL_SPI0_BIT;

static volatile uint32_t    *CLCK_IOCON_REG = &LPC_IOCON-> PIO0_9, // match GPIO
                            *DATA_IOCON_REG = &LPC_IOCON->PIO0_15, // match GPIO
                            *CLCK_SWM_REG   = &LPC_SWM->PINASSIGN3,
                            *DATA_SWM_REG   = &LPC_SWM->PINASSIGN4;

static const uint32_t       CLCK_SWM_MASK   = SWM_PINASSIGN3_SPI0_SCLK_IO_MASK,
                            DATA_SWM_MASK   = SWM_PINASSIGN4_SPI0_MOSI_IO_MASK;

static const uint8_t        CLCK_GPIO_NDX   = PIO_0_9_POS,   // match IOCON
                            DATA_GPIO_NDX   = PIO_0_15_POS,  // match IOCON
                            CLCK_SWM_POS    = SWM_PINASSIGN3_SPI0_SCLK_IO_POS,
                            DATA_SWM_POS    = SWM_PINASSIGN4_SPI0_MOSI_IO_POS;

static const uint32_t       CLCK_SWM_BITS   = CLCK_GPIO_NDX << CLCK_SWM_POS,
                            DATA_SWM_BITS   = DATA_GPIO_NDX << DATA_SWM_POS;

static const uint16_t       CLOCK_DIVISOR = 1;


#define NUM_APA102S         12
#define FIRST_APA102_NDX     1

static const uint32_t   APA102_START  = 0x00000000,
                        APA102_FINISH = 0x00000000,
                        APA102_BLACK  = 0xe1000000,
                        APA102_WHITE  = 0xe1080808;

static uint32_t     apa102s[NUM_APA102S + 2];




void display_init()
{
    apa102s[              0] = APA102_START ;
    apa102s[NUM_APA102S - 1] = APA102_FINISH;

    // enable switch matrix clock
    LPC_SYSCON->SYSAHBCLKCTRL |= SYS_AHB_CLK_CTRL_SWM_BIT;

    // configure mapping, SPI clock/data -> GPIO ports
    CHANGE_BITS(*CLCK_SWM_REG, CLCK_SWM_MASK, CLCK_SWM_BITS);
    CHANGE_BITS(*DATA_SWM_REG, DATA_SWM_MASK, DATA_SWM_BITS);

    // disable switch matrix clock
    LPC_SYSCON->SYSAHBCLKCTRL &= ~SYS_AHB_CLK_CTRL_SWM_BIT;

    // enable SPI
    LPC_SYSCON->SYSAHBCLKCTRL |= SYS_AHB_CLK_CTRL_SPI_BIT;

    // configure SPI
    //
    SPI->DIV = CLOCK_DIVISOR                          ;
    SPI->CFG = SPI_CFG_ENABLE_BIT | SPI_CFG_MASTER_BIT;
}





void display(
const uint16_t  keys_bits)
{
    uint32_t    bit = 1 << 11;

    for (uint8_t      key_ndx = FIRST_APA102_NDX               ;
                      key_ndx < FIRST_APA102_NDX + NUM_APA102S ;
                    ++key_ndx, bit >>= 1                        )
        if (keys_bits & bit) apa102s[key_ndx] = APA102_WHITE;
        else                 apa102s[key_ndx] = APA102_BLACK;


    uint8_t     lsbs = 0;
    uint16_t    half    ;

    for (uint8_t apa102_ndx = 0 ; apa102_ndx < NUM_APA102S + 2 ; ) {
        // wait until peripheral ready
        while (   (SPI->STAT & (SPI_STAT_TXRDY_BIT | SPI_STAT_STALLED_BIT))
               !=               SPI_STAT_TXRDY_BIT                        )
            asm("nop");

        if (lsbs ^= 1)
            half = (apa102s[apa102_ndx] & 0xffff0000) >> 16;
        else {
            half = apa102s[apa102_ndx] & 0x0000ffff;
            ++apa102_ndx                                   ;
        }

        SPI->TXDATCTL =   half
                        | SPI_TXDATCTL_EOT_BIT
                        | SPI_TXDATCTL_RXIGNORE_BIT
                        | SPI_TXDATCTL_LEN_16_BITS ;
    }
}
