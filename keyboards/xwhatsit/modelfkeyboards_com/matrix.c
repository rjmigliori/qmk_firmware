/* Copyright 2020 Purdea Andrei
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "quantum.h"

#define DAC_SCLK   B1
#define DAC_DIN    B2
#define DAC_SYNC_N B0

#define SHIFT_DIN  D4
#define SHIFT_OE   D5
#define SHIFT_SHCP D7
#define SHIFT_STCP D6
#define SHIFT_STCP_IO _SFR_IO_ADDR(PORTD)
#define SHIFT_STCP_BIT 6

#define KEYBOARD_SETTLE_TIME_US 8
#define DAC_SETTLE_TIME_US 8

#define REPS 15

// the following black magic ensures, that no matter how you optimize your C code, the reads can be done in a controlled time manner.
//#define READ_ROWS_PIN_1 ((uint8_t)(((uint16_t)&PINC) - 0x20))
//#define READ_ROWS_PIN_2 ((uint8_t)(((uint16_t)&PIND) - 0x20))
#define READ_ROWS_PIN_1 _SFR_IO_ADDR(PINC)
#define READ_ROWS_PIN_2 _SFR_IO_ADDR(PIND)
#define READ_ROWS_ASM_INSTRUCTIONS "in %[dest_row_1], %[ioreg_row_1]\n\tin %[dest_row_2], %[ioreg_row_2]"
#define READ_ROWS_OUTPUT_CONSTRAINTS [dest_row_1] "=&r" (dest_row_1), [dest_row_2] "=&r" (dest_row_2)
#define READ_ROWS_INPUT_CONSTRAINTS [ioreg_row_1] "I" (READ_ROWS_PIN_1), [ioreg_row_2] "I" (READ_ROWS_PIN_2)
#define READ_ROWS_LOCAL_VARS uint8_t dest_row_1, dest_row_2
#define READ_ROWS_VALUE ((dest_row_1 >> 4) | (dest_row_2 << 4))

static inline uint8_t read_rows(void)
{
    READ_ROWS_LOCAL_VARS;
    asm volatile (READ_ROWS_ASM_INSTRUCTIONS : READ_ROWS_OUTPUT_CONSTRAINTS : READ_ROWS_INPUT_CONSTRAINTS);
    return READ_ROWS_VALUE;
}

void dac_init(void)
{
    setPinOutput(DAC_SCLK);
    setPinOutput(DAC_DIN);
    setPinOutput(DAC_SYNC_N);
    writePin(DAC_SYNC_N, 1);
    writePin(DAC_SCLK, 0);
    writePin(DAC_SCLK, 1);
    writePin(DAC_SCLK, 0);
}

void dac_write_threshold(uint16_t value)
{
    value <<= 2; // The two LSB bits of this DAC are don't care.
    writePin(DAC_SYNC_N, 0);
    int i;
    for (i=0;i<16;i++)
    {
        writePin(DAC_DIN, (value >> 15) & 1);
        value <<= 1;
        writePin(DAC_SCLK, 1);
        writePin(DAC_SCLK, 0);
    }
    writePin(DAC_SYNC_N, 1);
    writePin(DAC_SCLK, 1);
    writePin(DAC_SCLK, 0);
    wait_us(DAC_SETTLE_TIME_US);
}

void shift_select_nothing(void)
{
    writePin(SHIFT_DIN, 0);
    int i;
    for (i=0;i<16;i++)
    {
        writePin(SHIFT_SHCP, 1);
        writePin(SHIFT_SHCP, 0);
    }
    writePin(SHIFT_STCP, 1);
    writePin(SHIFT_STCP, 0);
}

void shift_select_col_no_strobe(uint8_t col)
{
    int i;
    for (i=15; i>=0; i--)
    {
        writePin(SHIFT_DIN, !!(col == i));
        writePin(SHIFT_SHCP, 1);
        writePin(SHIFT_SHCP, 0);
    }
}

static inline void shift_select_col(uint8_t col)
{
    shift_select_col_no_strobe(col);
    writePin(SHIFT_STCP, 1);
    writePin(SHIFT_STCP, 0);
}

void shift_init(void)
{
    setPinOutput(SHIFT_DIN);
    setPinOutput(SHIFT_OE);
    setPinOutput(SHIFT_STCP);
    setPinOutput(SHIFT_SHCP);
    writePin(SHIFT_STCP, 0);
    writePin(SHIFT_SHCP, 0);
    shift_select_nothing();
    wait_us(KEYBOARD_SETTLE_TIME_US);
}

// the following function requires storage for 2 * (time + 1) bytes
// but returns valid data only in the first (time + 1) bytes
void test_multiple(uint8_t col, uint16_t time, uint8_t *array)
{
    shift_select_col_no_strobe(col);
    uint16_t index;
    READ_ROWS_LOCAL_VARS;
    uint8_t *arrayp = array;
    asm volatile (
             "ldi %A[index], 0"                 "\n\t"
             "ldi %B[index], 0"                 "\n\t"
             "cli"                              "\n\t"
             "sbi %[stcp_regaddr], %[stcp_bit]" "\n\t"
        "1:" READ_ROWS_ASM_INSTRUCTIONS         "\n\t"
             "st %a[arr]+, %[dest_row_1]"       "\n\t"
             "st %a[arr]+, %[dest_row_2]"       "\n\t"
             "adiw %A[index], 0x01"             "\n\t"
             "cp %A[index], %A[time]"           "\n\t"
             "cpc %B[index], %B[time]"          "\n\t"
             "brlo 1b"                          "\n\t"
             "sei"                              "\n\t"
             "cbi %[stcp_regaddr], %[stcp_bit]" "\n\t"
      : [arr] "=e" (arrayp),
        [index] "=&w" (index),
        READ_ROWS_OUTPUT_CONSTRAINTS
      : [time] "r" (time + 1),
        [stcp_regaddr] "I" (SHIFT_STCP_IO),
        [stcp_bit] "I" (SHIFT_STCP_BIT),
        READ_ROWS_INPUT_CONSTRAINTS,
        "0" (arrayp)
      : "memory" );
    uint16_t i, p0, p1;
    p0 = p1 = 0;
    for (i=0; i<=time; i++)
    {
        dest_row_1 = array[p0++];
        dest_row_2 = array[p0++];
        array[p1++] = READ_ROWS_VALUE;
    }
    shift_select_nothing();
    wait_us(KEYBOARD_SETTLE_TIME_US);
}

uint8_t test_single(uint8_t col, uint16_t time)
{
    shift_select_col_no_strobe(col);
    uint16_t index;
    READ_ROWS_LOCAL_VARS;
    uint8_t dummy_data;
    uint8_t *arrayp = &dummy_data;
    asm volatile (
             "ldi %A[index], 0"                 "\n\t"
             "ldi %B[index], 0"                 "\n\t"
             "cli"                              "\n\t"
             "sbi %[stcp_regaddr], %[stcp_bit]" "\n\t"
        "1:" READ_ROWS_ASM_INSTRUCTIONS         "\n\t"
             "st %a[arr], %[dest_row_1]"       "\n\t"
             "st %a[arr], %[dest_row_2]"       "\n\t"
             "adiw %A[index], 0x01"             "\n\t"
             "cp %A[index], %A[time]"           "\n\t"
             "cpc %B[index], %B[time]"          "\n\t"
             "brlo 1b"                          "\n\t"
             "sei"                              "\n\t"
             "cbi %[stcp_regaddr], %[stcp_bit]" "\n\t"
      : [arr] "=e" (arrayp),
        [index] "=&w" (index),
        READ_ROWS_OUTPUT_CONSTRAINTS
      : [time] "r" (time + 1),
        [stcp_regaddr] "I" (SHIFT_STCP_IO),
        [stcp_bit] "I" (SHIFT_STCP_BIT),
        READ_ROWS_INPUT_CONSTRAINTS,
        "0" (arrayp)
      : "memory" );
    shift_select_nothing();
    wait_us(KEYBOARD_SETTLE_TIME_US);
    return READ_ROWS_VALUE;
}

uint8_t test_col(uint8_t col, uint8_t *array)
{
    cli();
    uint8_t starting_state = read_rows();
    shift_select_col(col);
    int i;
    for (i=0;i<16;i++)
    {
        uint8_t data = read_rows();
        array[i] = data;
    }
    sei();
    shift_select_nothing();
    wait_us(KEYBOARD_SETTLE_TIME_US);
    return starting_state;
}

void test_col_print_data(uint8_t col)
{
    uprintf("%d: ", col);
    uint8_t data[16];
    uint8_t sums[(16+1) * 8];
    uint8_t i;
    for (i=0;i<(sizeof(sums)/sizeof(sums[0]));i++)
    {
        sums[i] = 0;
    }
    for (i=0;i<REPS;i++)
    {
        uint8_t st = test_col(col, data);
        uint8_t j;
        uint8_t ii = 0;
        uint8_t k;
        for (j=0;j<16;j++)
        {
            uint8_t dataj = data[j];
            for (k=0; k<8;k++)
            {
                sums[ii] += (dataj & 1);
                dataj >>= 1;
                ii += 1;
            }
        }
        for (k=0; k<8;k++)
        {
            sums[ii] += (st & 1);
            st >>= 1;
            ii += 1;
        }
    }
    for (i=16*8;i<17*8;i++)
    {
        uprintf("%X", sums[i]);
    }
    print(":");
    for (i=0;i<16*8;i++)
    {
        uprintf("%X", sums[i]);
    }
    print("\n");
}

#define NRTIMES 128
#define TESTATONCE 8
#define REPS_V2 15
void test_col_print_data_v2(uint8_t col)
{
    uprintf("%d: ", col);
    static uint8_t data[NRTIMES*2];
    static uint8_t sums[(TESTATONCE+1) * 8];
    uint8_t to_time = NRTIMES-1;
    uint8_t from_time = 0;
    while (from_time<NRTIMES-1)
    {
        if (to_time - from_time + 1 > TESTATONCE)
        {
            to_time = from_time + TESTATONCE - 1;
        }
        uint8_t curr_TESTATONCE = to_time - from_time + 1;
        uint8_t i;
        for (i=0;i<(sizeof(sums)/sizeof(sums[0]));i++)
        {
            sums[i] = 0;
        }
        for (i=0;i<REPS_V2;i++)
        {
            uint8_t st = read_rows();
            test_multiple(col, to_time, data);
            uint8_t j;
            uint8_t ii = 0;
            uint8_t k;
            for (j=0;j<curr_TESTATONCE;j++)
            {
                uint8_t dataj = data[j + from_time];
                for (k=0; k<8;k++)
                {
                    sums[ii] += (dataj & 1);
                    dataj >>= 1;
                    ii += 1;
                }
            }
            if (from_time == 0) {
                ii = TESTATONCE * 8;
                for (k=0; k<8;k++)
                {
                    sums[ii] += (st & 1);
                    st >>= 1;
                    ii += 1;
                }
            }
        }
        if (from_time == 0) {
            for (i=TESTATONCE*8;i<(TESTATONCE+1)*8;i++) {
                if (sums[i] > 0xf) {
                    print("?");
                } else {
                    uprintf("%X", sums[i]);
                }
            }
            print(":");
        }
        for (i=0;i<curr_TESTATONCE*8;i++)
        {
            if (sums[i] > 0xf) {
                print("?");
            } else {
                uprintf("%X", sums[i]);
            }
        }
        from_time = to_time + 1;
        to_time = NRTIMES - 1;
    }
    print("\n");
}


void test_v1(void) {
    int i;
    for (i=7;i>0;i--) {
        uprintf("Starting test in %d\n", i);
        wait_ms(1000);
    }
    uprintf("shift_init()");
    shift_init();
    uprintf(" DONE\n");
    uprintf("dac_init()");
    dac_init();
    uprintf(" DONE\n");
    int d;
    for (d=0;d<1024;d++)
    {
        uprintf("Testing threshold: %d\n", d);
        dac_write_threshold(d);
        int c;
        for (c=0; c<10;c++)
        {
            test_col_print_data(c);
        }
        test_col_print_data(15);
    }
    uprintf("TEST DONE\n");
    while(1);
}

void test_v2(void) {
    int i;
    for (i=7;i>0;i--) {
        uprintf("Starting test in %d\n", i);
        wait_ms(1000);
    }
    uprintf("shift_init()");
    shift_init();
    uprintf(" DONE\n");
    uprintf("dac_init()");
    dac_init();
    uprintf(" DONE\n");
    int d;
    for (d=90;d<=260;d++)
    {
        uprintf("Testing threshold: %d\n", d);
        dac_write_threshold(d);
        #if 1
            int c;
            for (c=0; c<10;c++)
            {
                test_col_print_data_v2(c);
            }
            test_col_print_data_v2(15);
        #else
            test_col_print_data_v2(0);
            test_col_print_data_v2(2);
            test_col_print_data_v2(6);
            test_col_print_data_v2(7);
            test_col_print_data_v2(15);
        #endif
    }
    uprintf("TEST DONE\n");
    while(1);
}


void real_keyboard_init_basic(void)
{
    uprintf("shift_init()");
    shift_init();
    uprintf(" DONE\n");
    uprintf("dac_init()");
    dac_init();
    uprintf(" DONE\n");
    dac_write_threshold(142);
}

void matrix_init_custom(void) {
    //test_v1();
    test_v2();
    real_keyboard_init_basic();
}

uint8_t previous_raw_matrix[11];

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    uint8_t col;
    uint8_t row;
    bool changed = false;
    for (row=0;row<8;row++)
    {
        current_matrix[row] = 0;
    }
    for (col=0;col<11;col++)
    {
        uint8_t real_col = col;
        if (col == 10) real_col = 15;
        uint8_t data[16];
        test_col(real_col, data);
        uint8_t d = data[2];
        if (previous_raw_matrix[col] != d) changed = true;
        previous_raw_matrix[col] = d;
        for (row=0;row<8;row++)
        {
            current_matrix[7-row] |= (((uint16_t)(d & 1)) << col);
            d >>= 1;
        }
    }
    return changed;
}
