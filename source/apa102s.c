
            uint32_t    bit;



#ifdef USE_SK9822
            for (uint8_t ndx = 0 ; ndx < NUM_IRGBS ; ++ndx)
                IRBGS[ndx] = DIM_LED;

            bit = 0x1;

            for (uint8_t row_ndx = 0 ; row_ndx < NUM_IOS - 1 ; ++row_ndx) {
                uint32_t    led_color = (  row_ndx == 0 ? RED_LED
                                         : row_ndx == 1 ? GRN_LED
                                         :                BLU_LED);

                for (uint8_t col_ndx = 0 ; col_ndx < NUM_IOS ; ++col_ndx) {
                    if (mppd_down & bit) IRBGS[col_ndx] |=  led_color;
                    else                 IRBGS[col_ndx] &= ~led_color;
                    bit <<= 1;
                }
            }

            SK9822::send_pixels(clock_data, IRBGS, NUM_IRGBS, NUM_IRGBS, 0);
#endif
