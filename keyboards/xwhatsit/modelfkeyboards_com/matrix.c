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

#define HARDCODED_SAMPLE_TIME 4

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

#define TRACKING_TEST_TIME 4
// Key 1 is the always non-pressed key under the space bar to the right.
#define TRACKING_KEY_1_COL 6
#define TRACKING_KEY_1_ROW 4
// Key 2 is the always-pressed calibration pad to the far right-bottom of the keyboard. (both on F62 and F77)
#define TRACKING_KEY_2_COL 15
#define TRACKING_KEY_2_ROW 6
// Key 3 is the F key
#define TRACKING_KEY_3_COL 2
#define TRACKING_KEY_3_ROW 5
// Key 4 is the half of the split backspace that is unused if the user has a normal backspace.
#define TRACKING_KEY_4_COL 7
#define TRACKING_KEY_4_ROW 3
// Key 5 is hidden key next to the left shift, which is only used in ISO layouts.
#define TRACKING_KEY_5_COL 0
#define TRACKING_KEY_5_ROW 7

#define TRACKING_REPS 16

static uint16_t measure_middle(uint8_t col, uint8_t row, uint8_t time, uint8_t reps)
{
    uint8_t reps_div2 = reps / 2;
    uint16_t min = 0, max = 1023;
    while (min < max)
    {
        uint16_t mid = (min + max) / 2;
        dac_write_threshold(mid);
        uint8_t sum = 0;
        uint8_t i;
        for (i=0;i<reps;i++)
        {
            sum += (test_single(col, time) >> row) & 1;
        }
        if (sum < reps_div2)
        {
            max = mid - 1;
        } else if (sum > reps_div2) {
            min = mid + 1;
        } else return mid;
    }
    return min;
}

void tracking_test(void)
{
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
    while (1) {
        uint16_t key1 = measure_middle(TRACKING_KEY_1_COL, TRACKING_KEY_1_ROW, TRACKING_TEST_TIME, TRACKING_REPS);
        uint16_t key2 = measure_middle(TRACKING_KEY_2_COL, TRACKING_KEY_2_ROW, TRACKING_TEST_TIME, TRACKING_REPS);
        uint16_t key3 = measure_middle(TRACKING_KEY_3_COL, TRACKING_KEY_3_ROW, TRACKING_TEST_TIME, TRACKING_REPS);
        uint16_t key4 = measure_middle(TRACKING_KEY_4_COL, TRACKING_KEY_4_ROW, TRACKING_TEST_TIME, TRACKING_REPS);
        uint16_t key5 = measure_middle(TRACKING_KEY_5_COL, TRACKING_KEY_5_ROW, TRACKING_TEST_TIME, TRACKING_REPS);
        uprintf("%u, %u, %u, %u, %u\n", key1, key2, key3, key4, key5);
    }
}

#define KEYMAP_ROW_TO_PHYSICAL_ROW(row) (7-(row))
#define PHYSICAL_ROW_TO_KEYMAP_ROW(row) (7-(row))
#define KEYMAP_COL_TO_PHYSICAL_COL(col) (((col) == 10)?15:(col))

uint16_t calibration_measure_all_valid_keys(uint8_t time, uint8_t reps, bool looking_for_all_zero)
{
    uint16_t min = 0, max = 1023;
    while (min < max)
    {
        uint16_t mid = (min + max) / 2;
        if (!looking_for_all_zero) {
            mid = (min + max + 1) / 2;
        }
        dac_write_threshold(mid);
        uint8_t col;
        for (col = 0; col < MATRIX_COLS; col++)
        {
            uint8_t valid_physical_rows = 0;
            uint8_t row;
            for (row=0; row < MATRIX_ROWS; row++)
            {
                if (pgm_read_byte(&keymaps[0][row][col]) != KC_NO)
                {
                    valid_physical_rows |= (1 << KEYMAP_ROW_TO_PHYSICAL_ROW(row)); // convert keymap row to physical row
                }
            }
            uint8_t physical_col = KEYMAP_COL_TO_PHYSICAL_COL(col);
            uint8_t i;
            for (i=0;i<reps;i++) {
                if (looking_for_all_zero)
                {
                    uint8_t all_zero = (test_single(physical_col, time) & valid_physical_rows) == 0;
                    if (!all_zero) {
                        min = mid + 1;
                        goto next_binary_search;
                    }
                } else {
                    uint8_t all_ones = (test_single(physical_col, time) & valid_physical_rows) == valid_physical_rows;
                    if (!all_ones) {
                        max = mid - 1;
                        goto next_binary_search;
                    }
                }
            }
        }
        if (looking_for_all_zero) {
            max = mid;
        } else {
            min = mid;
        }
        next_binary_search:;
    }
    return min;
}

#define CAL_ENABLED
#define CAL_DEBUG
#define CAL_INIT_REPS 16
#define CAL_EACHKEY_REPS 16
#define CAL_BINS 3
#define CAL_THRESHOLD_OFFSET 12

#if defined(CAL_ENABLED)
#if defined(BOOTMAGIC_ENABLE) || defined(BOOTMAGIC_LITE)
#error "Calibration is not supported in conjunction with BOOTMAGIC, because calibration requires that no keys are pressed while the keyboard is plugged in"
#endif
#endif

uint16_t cal_thresholds[CAL_BINS];
matrix_row_t assigned_to_threshold[CAL_BINS][MATRIX_ROWS];
uint16_t cal_tr_allzero;
uint16_t cal_tr_allone;
void calibration(void)
{
    uint16_t cal_thresholds_max[CAL_BINS]={0xFFFFU,0xFFFFU,0xFFFFU};
    uint16_t cal_thresholds_min[CAL_BINS]={0xFFFFU,0xFFFFU,0xFFFFU};
    cal_tr_allzero = calibration_measure_all_valid_keys(HARDCODED_SAMPLE_TIME, CAL_INIT_REPS, true);
    cal_tr_allone = calibration_measure_all_valid_keys(HARDCODED_SAMPLE_TIME, CAL_INIT_REPS, false);
    uint16_t max = (cal_tr_allzero == 0) ? 0 : (cal_tr_allzero - 1);
    uint16_t min = cal_tr_allone + 1;
    if (max < min) max = min;
    uint16_t d = max - min;
    uint8_t i;
    for (i=0;i<CAL_BINS;i++) {
        cal_thresholds[i] = min + (d * (2 * i + 1)) / 2 / CAL_BINS;
    }
    uint8_t col;
    for (col = 0; col < MATRIX_COLS; col++) {
        uint8_t physical_col = KEYMAP_COL_TO_PHYSICAL_COL(col);
        uint8_t row;
        for (row = 0; row < MATRIX_ROWS; row++) {
            if (pgm_read_byte(&keymaps[0][row][col]) != KC_NO) {
                uint16_t threshold = measure_middle(physical_col, KEYMAP_ROW_TO_PHYSICAL_ROW(row), HARDCODED_SAMPLE_TIME, CAL_EACHKEY_REPS);
                uint8_t besti = 0;
                uint16_t best_diff = (uint16_t)abs(threshold - cal_thresholds[besti]);
                for (i=1;i<CAL_BINS;i++) {
                    uint16_t this_diff = (uint16_t)abs(threshold - cal_thresholds[i]);
                    if (this_diff < best_diff)
                    {
                        best_diff = this_diff;
                        besti = i;
                    }
                }
                assigned_to_threshold[besti][row] |= (1 << col);
                if ((cal_thresholds_max[besti] = 0xFFFFU) || (cal_thresholds_max[besti] < threshold)) cal_thresholds_max[besti] = threshold;
                if ((cal_thresholds_min[besti] = 0xFFFFU) || (cal_thresholds_min[besti] > threshold)) cal_thresholds_min[besti] = threshold;
            }
        }
    }
    for (i=0;i<CAL_BINS;i++) {
        if ((cal_thresholds_max[i] == 0xFFFFU) || (cal_thresholds_min[i] == 0xFFFFU)) {
            cal_thresholds[i] += CAL_THRESHOLD_OFFSET;
        } else {
            cal_thresholds[i] = (cal_thresholds_max[i] + cal_thresholds_min[i]) / 2 + CAL_THRESHOLD_OFFSET;
        }
    }
}

void real_keyboard_init_basic(void)
{
    uprintf("shift_init()");
    shift_init();
    uprintf(" DONE\n");
    uprintf("dac_init()");
    dac_init();
    uprintf(" DONE\n");
    #if defined(CAL_ENABLED)
    calibration();
    #else
    dac_write_threshold(142);
    dac_write_threshold(142);
    dac_write_threshold(142);
    #endif
}

void matrix_init_custom(void) {
    //test_v2();
    //tracking_test();
    real_keyboard_init_basic();
}

matrix_row_t previous_matrix[MATRIX_ROWS];
#if defined(CAL_ENABLED) && defined(CAL_DEBUG)
bool cal_stats_printed = false;
#endif

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    uint8_t col, row, cal;
    #if defined(CAL_ENABLED) && defined(CAL_DEBUG)
    if (!cal_stats_printed)
    {
        uint32_t time = timer_read32();
        if (time >= 10 * 1000UL) { // after 10 seconds
            uprintf("Cal All Zero = %u, Cal All Ones = %u\n", cal_tr_allzero, cal_tr_allone);
            for (cal=0;cal<CAL_BINS;cal++)
            {
                uprintf("Cal bin %u, Threshold=%u Assignments:\n", cal, cal_thresholds[cal]);
                for (row=0;row<MATRIX_ROWS;row++)
                {
                    uprintf("0x%02X\n", assigned_to_threshold[cal][row]);
                }
            }
            cal_stats_printed = true;
        }
    }
    #endif
    for (row=0;row<8;row++)
    {
        current_matrix[row] = 0;
    }
    #if defined(CAL_ENABLED)
    for (cal=0;cal<CAL_BINS;cal++) {
        dac_write_threshold(cal_thresholds[cal]);
        for (col=0;col<MATRIX_COLS;col++) {
            uint8_t real_col = KEYMAP_COL_TO_PHYSICAL_COL(col);
            uint8_t d;
            uint8_t d_tested = 0;
            for (row=0;row<MATRIX_ROWS;row++) {
                if (assigned_to_threshold[cal][row] & (1 << col))
                {
                    if (!d_tested)
                    {
                        d = test_single(real_col, HARDCODED_SAMPLE_TIME);
                        d_tested = 1;
                    }
                    uint8_t physical_row = KEYMAP_ROW_TO_PHYSICAL_ROW(row);
                    current_matrix[row] |= ((d >> physical_row) & 1) << col;
                }
            }
        }
    }
    #else
    for (col=0;col<MATRIX_COLS;col++)
    {
        uint8_t real_col = KEYMAP_COL_TO_PHYSICAL_COL(col);
        uint8_t d = test_single(real_col, HARDCODED_SAMPLE_TIME);
        for (row=0;row<MATRIX_ROWS;row++)
        {
            current_matrix[PHYSICAL_ROW_TO_KEYMAP_ROW(row)] |= (((uint16_t)(d & 1)) << col);
            d >>= 1;
        }
    }
    #endif
    bool changed = false;
    for (row=0;row<MATRIX_ROWS;row++)
    {
        if (previous_matrix[row] != current_matrix[row]) changed = true;
        previous_matrix[row] = current_matrix[row];
    }
    return changed;
}
