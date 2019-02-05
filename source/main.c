#include <stdint.h>

#include <LPC82x.h>

#include <lpc824_bits.h>




// CPU speed
#define MAIN_CLOCK_DIVISOR      1
#define MAIN_CLOCK_MHZ         12   // (native 12 MHz) / MAIN_CLOCK_DIVISOR
#define SCAN_DELAY_MS        1000
#define TICKS_PER_LOOP         16   // rough estimate
#define SCAN_DELAY_LOOPS    (  ((MAIN_CLOCK_MHZ * 1000000) / SCAN_DELAY_MS) \
                             / TICKS_PER_LOOP                              )


// keypad connections
//
#define BRWN_GPIO_NDX   PIO_0_1_POS
#define PRPL_GPIO_NDX   PIO_0_23_POS
#define BLUE_GPIO_NDX   PIO_0_14_POS
#define GRAY_GPIO_NDX   PIO_0_0_POS

#define BRWN_GPIO_BIT   (1 << BRWN_GPIO_NDX)
#define PRPL_GPIO_BIT   (1 << PRPL_GPIO_NDX)
#define BLUE_GPIO_BIT   (1 << BLUE_GPIO_NDX)
#define GRAY_GPIO_BIT   (1 << GRAY_GPIO_NDX)


static const uint8_t         GPIO_NDXS[]  = {BRWN_GPIO_NDX,
                                             PRPL_GPIO_NDX,
                                             BLUE_GPIO_NDX,
                                             GRAY_GPIO_NDX};
static const uint32_t       GPIO_BITS[]   = {BRWN_GPIO_BIT,
                                             PRPL_GPIO_BIT,
                                             BLUE_GPIO_BIT,
                                             GRAY_GPIO_BIT};
static volatile uint32_t    *IOCON_REGS[] = {&LPC_IOCON->PIO0_1 ,
                                             &LPC_IOCON->PIO0_2 ,
                                             &LPC_IOCON->PIO0_14,
                                             &LPC_IOCON->PIO0_0 };

#define NUM_GPIOS   (sizeof(GPIO_NDXS) / sizeof(uint8_t))
#define NUM_KEYS    (NUM_GPIOS * (NUM_GPIOS - 1))


#ifdef SWITCHABLE_KEYPAD
typedef struct mapping {
const uint16_t  orignl,
                mapped;
} Mapping;

static const Mapping    mapping[] = {{0x0200, 0x0001},
                                     {0x0001, 0x0002},
                                     {0x0002, 0x0004},
                                     {0x0004, 0x0008},

                                     {0x0008, 0x0010},
                                     {0x0400, 0x0020},
                                     {0x0010, 0x0040},
                                     {0x0020, 0x0080},

                                     {0x0040, 0x0100},
                                     {0x0080, 0x0200},
                                     {0x0800, 0x0400},
                                     {0x0100, 0x0800}};
#endif




uint32_t    plex(const    uint8_t   const   *gpio_ndxs ,
                 volatile uint32_t* const   *iocon_regs,
                 const    uint8_t            num_gpios );

uint32_t    scan(const    uint8_t   const   *gpio_ndxs ,
                 const    uint32_t  const   *gpio_bits ,
                 volatile uint32_t* const   *iocon_regs,
                 const    uint8_t            num_gpios );

#ifdef USE_DISPLAY
            void    display_init(                        );
            void    display     (const uint16_t keys_down);
#endif




int main()
{
    // init main clock
    LPC_SYSCON->SYSAHBCLKDIV = MAIN_CLOCK_DIVISOR;

#ifdef USE_DISPLAY
    // must be before markoplex.c:init() because disables SWM clock
    display_init();
#endif

    // init charlieplex or markoplex
    plex(GPIO_NDXS, IOCON_REGS, NUM_GPIOS);



    // run
    //
    volatile uint32_t   scans     = 0,
                        changes   = 0;
    volatile uint16_t   prev_down = 0,
                        keys_down    ,
                        keys_mapd    ;

    while (1) {
        keys_down = scan(GPIO_NDXS, GPIO_BITS, IOCON_REGS, NUM_GPIOS );

        if (keys_down != prev_down) {
#ifdef SWITCHABLE_KEYPAD
            keys_mapd = 0;
            for (uint8_t   ndx = 0                                 ;
                           ndx < sizeof(mapping) / sizeof(Mapping) ;
                         ++ndx                                      )
                if (keys_down &  mapping[ndx].orignl) {
                    keys_mapd |= mapping[ndx].mapped;
                    asm("nop");   /* DEBUG */
                }
#else
        keys_mapd = keys_down;
#endif

#ifdef USE_DISPLAY
            display(keys_mapd);
#endif  // ifdef USE_DISPLAY

            keys: asm("nop");    // for GDB dprintf

            // set for next loop
            prev_down = keys_down;

            ++changes;
        }

        for (uint32_t delay = 0 ; delay < SCAN_DELAY_LOOPS ; ++delay)
            asm("nop");

        ++scans;
    }
}
