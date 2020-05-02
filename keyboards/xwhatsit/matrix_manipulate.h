#ifndef MATRIX_MANIPULATE_H
#define MATRIX_MANIPULATE_H

#include "quantum.h"

// Contains stuff used to manipulate the matrix using the util.
// These are defined in matrix.c. This file is not called matrix.h to avoid conflict with qmk-native matrix.h

extern bool keyboard_scan_enabled;
void matrix_scan_raw(matrix_row_t current_matrix[]);
extern uint16_t cal_thresholds[CAPSENSE_CAL_BINS];
extern matrix_row_t assigned_to_threshold[CAPSENSE_CAL_BINS][MATRIX_ROWS];

#endif

