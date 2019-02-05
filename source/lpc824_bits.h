#ifndef LPC8XX_BITS_H
#define LPC8XX_BITS_H

#define CHANGE_BITS(REGISTER, MASK, BITS) \
    (REGISTER) = ((REGISTER) & ~(MASK)) | (BITS)


#define SYS_AHB_CLK_CTRL_GPIO_BIT   (1 <<  6)
#define SYS_AHB_CLK_CTRL_SWM_BIT    (1 <<  7)
#define SYS_AHB_CLK_CTRL_SPI0_BIT   (1 << 11)
#define SYS_AHB_CLK_CTRL_IOCON_BIT  (1 << 18)
#define SYS_AHB_CLK_CTRL_ACMP_BIT   (1 << 19)

#define SYS_CON_PDRUNCFG_ACMP_PD_BIT    (1 << 15)


#define SWM_PINASSIGN3_SPI0_SCLK_IO_POS     24
#define SWM_PINASSIGN4_SPI0_MOSI_IO_POS      0

#define SWM_PINASSIGN3_SPI0_SCLK_IO_MASK    (    0xff                           \
                                             << SWM_PINASSIGN3_SPI0_SCLK_IO_POS)
#define SWM_PINASSIGN4_SPI0_MOSI_IO_MASK    (    0xff                           \
                                             << SWM_PINASSIGN4_SPI0_MOSI_IO_POS)


#define SWM_PINENABLE0_ACMP_I1_EN_BIT        (1 << 0)
#define SWM_PINENABLE0_ACMP_I2_EN_BIT        (1 << 1)
#define SWM_PINENABLE0_ACMP_I3_EN_BIT        (1 << 2)
#define SWM_PINENABLE0_ACMP_I4_EN_BIT        (1 << 3)


#define PIO_0_0_POS       0
#define PIO_0_1_POS       1
#define PIO_0_2_POS       2
#define PIO_0_3_POS       3
#define PIO_0_4_POS       4
#define PIO_0_5_POS       5
#define PIO_0_6_POS       6
#define PIO_0_7_POS       7
#define PIO_0_8_POS       8
#define PIO_0_9_POS       9
#define PIO_0_10_POS     10
#define PIO_0_11_POS     11
#define PIO_0_12_POS     12
#define PIO_0_13_POS     13
#define PIO_0_14_POS     14
#define PIO_0_15_POS     15
#define PIO_0_16_POS     16
#define PIO_0_17_POS     17
#define PIO_0_18_POS     18
#define PIO_0_19_POS     19
#define PIO_0_20_POS     20
#define PIO_0_21_POS     21
#define PIO_0_22_POS     22
#define PIO_0_23_POS     23


#define IOCON_MODES_PULL_POS         3
#define IOCON_MODES_PULL_MASK       (0x3 << IOCON_MODES_PULL_POS)
#define IOCON_MODES_PULL_NONE       (0x0 << IOCON_MODES_PULL_POS)
#define IOCON_MODES_PULL_DOWN       (0x1 << IOCON_MODES_PULL_POS)


#define SPI_CFG_ENABLE_BIT          (1 << 0)
#define SPI_CFG_MASTER_BIT          (1 << 2)

#define SPI_STAT_TXRDY_BIT          (1 << 1)
#define SPI_STAT_STALLED_BIT        (1 << 6)

#define SPI_TXDATCTL_EOT_BIT        (  1 << 20)
#define SPI_TXDATCTL_RXIGNORE_BIT   (  1 << 22)
#define SPI_TXDATCTL_LEN_16_BITS    (0xf << 24)


#define CMP_CTRL_COMP_VP_SEL_MASK            (0x7 <<  8)
#define CMP_CTRL_COMP_VP_SEL_LADDER_BITS     (0x0 <<  8)
#define CMP_CTRL_COMP_VP_SEL_ACMP_I1_BITS    (0x1 <<  8)
#define CMP_CTRL_COMP_VP_SEL_ACMP_I2_BITS    (0x2 <<  8)
#define CMP_CTRL_COMP_VP_SEL_ACMP_I3_BITS    (0x3 <<  8)
#define CMP_CTRL_COMP_VP_SEL_ACMP_I4_BITS    (0x4 <<  8)
#define CMP_CTRL_COMP_VM_SEL_LADDER_BITS     (  0 << 11)

#define CMP_CTRL_EDGECLR_BIT                (1 << 20)
#define CMP_CTRL_COMPSTAT_BIT               (1 << 21)

#define CMP_LADDER_LADEN_BIT                (   1 << 0)
#define CMP_LADDER_LADSEL_MASK              (0x1f << 1)
#define CMP_LADDER_LADREF_VDD_BIT           (   0 << 6)

#endif  /* ifndef LPC8XX_BITS_H */
