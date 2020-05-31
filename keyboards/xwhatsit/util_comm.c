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
#include "raw_hid.h"
#include "util_comm.h"
#include "matrix_manipulate.h"
#include <string.h>
#include <tmk_core/common/eeprom.h>

#if defined(KEYBOARD_SHARED_EP) && defined(RAW_ENABLE)
#error "Enabling the KEYBOARD_SHARED_EP will make the util be unable to communicate with the firmware, because due to hidapi limiations, the util can't figure out which interface to talk to, so it hardcodes interface zero."
#endif

#ifndef RAW_EPSIZE
#define RAW_EPSIZE 32
#endif

extern const char *KEYBOARD_FILENAME; // This must be defined in keyboard_name.c to equal the filename. This is sent back to the PC-side software for it to determine which keyboard we are using.

static const uint8_t magic[] = UTIL_COMM_MAGIC;

void raw_hid_receive(uint8_t *data, uint8_t length) {
    if (0 != memcmp(data, magic, sizeof(magic))) {
        return;
    }
    uint8_t response[RAW_EPSIZE];
    memcpy(response, magic, sizeof(magic));
    response[2] = UTIL_COMM_RESPONSE_ERROR;
    switch (data[2])
    {
        case UTIL_COMM_GET_VERSION:
            response[2] = UTIL_COMM_RESPONSE_OK;
            response[3] = UTIL_COMM_VERSION_MAJOR;
            response[4] = UTIL_COMM_VERSION_MID;
            response[5] = (UTIL_COMM_VERSION_MINOR >> 8) & 0xff;
            response[6] = (UTIL_COMM_VERSION_MINOR >> 0) & 0xff;
            break;
        case UTIL_COMM_DISABLE_KEYBOARD:
            response[2] = UTIL_COMM_RESPONSE_OK;
            response[3] = (uint8_t) keyboard_scan_enabled;
            keyboard_scan_enabled = 0;
            break;
        case UTIL_COMM_ENABLE_KEYBOARD:
            response[2] = UTIL_COMM_RESPONSE_OK;
            response[3] = (uint8_t) keyboard_scan_enabled;
            keyboard_scan_enabled = 1;
            break;
        case UTIL_COMM_ENTER_BOOTLOADER:
            keyboard_scan_enabled = 0;
            wait_ms(10);
            bootloader_jump();
            // we should not be reaching the following:
            wait_ms(10);
            response[2] = UTIL_COMM_RESPONSE_OK;
            break;
        case UTIL_COMM_GET_KEYSTATE:
            response[2] = UTIL_COMM_RESPONSE_OK;
            #if 1
                matrix_scan_raw((matrix_row_t *)&response[3]); // AVR doesn't require not-byte values to be aligned, so this is correct, and it's an optimization.
            #else
                {
                    matrix_row_t current_matrix[MATRIX_ROWS];
                    matrix_scan_raw(current_matrix);
                    memcpy(&response[3], current_matrix, sizeof(current_matrix));
                }
            #endif
            break;
        case UTIL_COMM_GET_THRESHOLDS:
            response[2] = UTIL_COMM_RESPONSE_OK;
            #if CAPSENSE_CAL_ENABLED
            response[3] = CAPSENSE_CAL_BINS;
            {
                const uint8_t cal_bin = data[3];
                response[4] = cal_thresholds[cal_bin] & 0xff;
                response[5] = (cal_thresholds[cal_bin] >> 8) & 0xff;
                memcpy(&response[6], assigned_to_threshold[cal_bin], sizeof(assigned_to_threshold[cal_bin]));
            }
            #else
            response[3] = 0;
            response[4] = (CAPSENSE_HARDCODED_THRESHOLD) & 0xff;
            response[5] = ((CAPSENSE_HARDCODED_THRESHOLD) >> 8) & 0xff;
            #endif
            break;
        case UTIL_COMM_GET_KEYBOARD_FILENAME:
            {
                response[2] = UTIL_COMM_RESPONSE_OK;
                if (data[3] >= strlen(KEYBOARD_FILENAME) + 1)
                {
                    response[3] = 0;
                } else {
                    const char *substring = KEYBOARD_FILENAME + data[3];
                    size_t substring_length = strlen(substring) + 1;
                    if (substring_length > RAW_EPSIZE - 3) substring_length = RAW_EPSIZE - 3;
                    memcpy(&response[3], substring, substring_length);
                }
                break;
            }
        case UTIL_COMM_ERASE_EEPROM:
            {
                response[2] = UTIL_COMM_RESPONSE_OK;
                uint16_t addr;
                for (addr=0; addr<E2END; addr += 1)
                {
                    eeprom_update_byte((uint8_t*)addr, 0xff);
                }
                break;
            }
        case UTIL_COMM_GET_SIGNAL_VALUE:
            {
                response[2] = UTIL_COMM_RESPONSE_OK;
                uint8_t col = data[3];
                uint8_t row = data[4];
                uint8_t count = data[5];
                int i;
                for (i=0;i<count;i++)
                {
                    uint16_t value = measure_middle_keymap_coords(col, row, CAPSENSE_HARDCODED_SAMPLE_TIME, 16);
                    response[3+i*2] = value & 0xff;
                    response[3+i*2+1] = (value >> 8) & 0xff;
                    col += 1;
                    if (col >= MATRIX_COLS) {
                        col -= MATRIX_COLS;
                        row += 1;
                    }
                    if (row >= MATRIX_ROWS)
                    {
                        break;
                    }
                }
                break;
            }
        case UTIL_COMM_GET_KEYBOARD_DETAILS:
            {
                response[2] = UTIL_COMM_RESPONSE_OK;
                response[3] = MATRIX_COLS;
                response[4] = MATRIX_ROWS;
                #if defined(CONTROLLER_IS_XWHATSIT_BEAMSPRING_REV_4)
                response[5] = 1;
                #elif defined(CONTROLLER_IS_XWHATSIT_MODEL_F_OR_WCASS_MODEL_F)
                response[5] = 2;
                #else
                response[5] = 0;
                #endif
                response[6] = CAPSENSE_KEYBOARD_SETTLE_TIME_US;
                response[7] = CAPSENSE_DAC_SETTLE_TIME_US;
                response[8] = CAPSENSE_HARDCODED_SAMPLE_TIME;
                response[9] = CAPSENSE_CAL_ENABLED;
                break;
            }
        default:
            break;
    }
    raw_hid_send(response, sizeof(response));
}
