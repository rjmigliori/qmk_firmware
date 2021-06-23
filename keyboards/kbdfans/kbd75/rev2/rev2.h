#pragma once

#include "kbd75.h"

/* LAYOUT
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
 * │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │
 * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
 * │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┼───┤
 * │1.5U │   │   │   │   │   │   │   │   │   │   │   │   │1.5U │   │
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┼───┤
 * │1.75U │   │   │   │   │   │   │   │   │   │   │   │2.25U   │   │
 * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┼───┤
 * │2.25U   │   │   │   │   │   │   │   │   │   │   │1.75U │   │   │
 * ├────┬───┴┬──┴─┬─┴───┴──┬┴───┼───┴───┴──┬┴──┬┴──┬┴──┬───┼───┼───┤
 * │1.25│1.25│1.25│2.25U   │1.25│2.75U     │   │   │   │   │   │   │
 * └────┴────┴────┴────────┴────┴──────────┴───┴───┴───┴───┴───┴───┘
 */

// LAYOUT for all possible switch positions on a KBD75 rev 1
#define LAYOUT( \
	K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, K014, K015, \
	K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, K115, \
	K200,       K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, K215, \
	K300,       K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K313,       K315, \
	K400, K401, K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K413, K414, K415, \
	K500, K501,       K503, K504,       K506,       K508,       K510, K511, K512, K513, K514, K515  \
) { \
	{ K000,  K001,  K002,  K003,  K004,  K005,  K006,  K007,  K008,  K009,  K010,  K011,  K012,  K013,  K014,  K015 }, \
	{ K100,  K101,  K102,  K103,  K104,  K105,  K106,  K107,  K108,  K109,  K110,  K111,  K112,  K113,  K114,  K115 }, \
	{ K200,  KC_NO, K202,  K203,  K204,  K205,  K206,  K207,  K208,  K209,  K210,  K211,  K212,  K213,  K214,  K215 }, \
	{ K300,  KC_NO, K302,  K303,  K304,  K305,  K306,  K307,  K308,  K309,  K310,  K311,  K312,  K313,  KC_NO, K315 }, \
	{ K400,  K401,  K402,  K403,  K404,  K405,  K406,  K407,  K408,  K409,  K410,  K411,  KC_NO, K413,  K414,  K415 }, \
	{ K500,  K501,  KC_NO, K503,  K504,  KC_NO, K506,  KC_NO, K508,  KC_NO, K510,  K511,  K512,  K513,  K514,  K515 }  \
}


/* 1U bottom row ─────────────────────────── ↓ ─ ↓ ─ ↓
 * ┌────┬────┬────┬────────────────────────┬───┬───┬───┬───┬───┬───┐
 * │1.25│1.25│1.25│6.25U                   │1U │1U │1U │1U │1U │1U │
 * └────┴────┴────┴────────────────────────┴───┴───┴───┴───┴───┴───┘
 */

#define LAYOUT_ansi_1u( \
	K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, K014, K015, \
	K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112,       K114, K115, \
	K200,       K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, K215, \
	K300,       K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K313,       K315, \
	K400,       K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K413, K414, K415, \
	K500, K501,       K503,             K506,                   K510, K511, K512, K513, K514, K515  \
) { \
	{ K000,  K001,  K002,  K003,  K004,  K005,  K006,  K007,  K008,  K009,  K010,  K011,  K012,  K013,  K014,  K015 }, \
	{ K100,  K101,  K102,  K103,  K104,  K105,  K106,  K107,  K108,  K109,  K110,  K111,  K112,  KC_NO, K114,  K115 }, \
	{ K200,  KC_NO, K202,  K203,  K204,  K205,  K206,  K207,  K208,  K209,  K210,  K211,  K212,  K213,  K214,  K215 }, \
	{ K300,  KC_NO, K302,  K303,  K304,  K305,  K306,  K307,  K308,  K309,  K310,  K311,  K312,  K313,  KC_NO, K315 }, \
	{ K400,  KC_NO, K402,  K403,  K404,  K405,  K406,  K407,  K408,  K409,  K410,  K411,  KC_NO, K413,  K414,  K415 }, \
	{ K500,  K501,  KC_NO, K503,  KC_NO, KC_NO, K506,  KC_NO, KC_NO, KC_NO, K510,  K511,  K512,  K513,  K514,  K515 }  \
}

#define LAYOUT_75_ansi LAYOUT_ansi_1u

#define LAYOUT_iso_1u( \
	K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, K014, K015, \
	K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112,       K114, K115, \
	K200,       K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K313, K215, \
	K300,       K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K214,       K315, \
	K400, K401, K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K413, K414, K415, \
	K500, K501,       K503,             K506,                   K510, K511, K512, K513, K514, K515  \
) { \
	{ K000,  K001,  K002,  K003,  K004,  K005,  K006,  K007,  K008,  K009,  K010,  K011,  K012,  K013,  K014,  K015 }, \
	{ K100,  K101,  K102,  K103,  K104,  K105,  K106,  K107,  K108,  K109,  K110,  K111,  K112,  KC_NO, K114,  K115 }, \
	{ K200,  KC_NO, K202,  K203,  K204,  K205,  K206,  K207,  K208,  K209,  K210,  K211,  K212,  K213,  K214,  K215 }, \
	{ K300,  KC_NO, K302,  K303,  K304,  K305,  K306,  K307,  K308,  K309,  K310,  K311,  K312,  K313,  KC_NO, K315 }, \
	{ K400,  K401,  K402,  K403,  K404,  K405,  K406,  K407,  K408,  K409,  K410,  K411,  KC_NO, K413,  K414,  K415 }, \
	{ K500,  K501,  KC_NO, K503,  KC_NO, KC_NO, K506,  KC_NO, KC_NO, KC_NO, K510,  K511,  K512,  K513,  K514,  K515 }  \
}

#define LAYOUT_75_iso( \
	K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, K014, K015, \
	K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112,       K114, K115, \
	K200,       K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213,       K215, \
	K300,       K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K214, K313, K315, \
	K400, K401, K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K413, K414, K415, \
	K500, K501,       K503,             K506,                   K510, K511, K512, K513, K514, K515  \
) LAYOUT_iso_1u( \
	K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, K014, K015, \
	K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112,       K114, K115, \
	K200,       K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K313, K215, \
	K300,       K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K214,       K315, \
	K400, K401, K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K413, K414, K415, \
	K500, K501,       K503,             K506,                   K510, K511, K512, K513, K514, K515  \
)


/* normal bottom row ──────────────────────── ↓ ─── ↓
 * ┌────┬────┬────┬────────────────────────┬─────┬─────┬───┬───┬───┐
 * │1.25│1.25│1.25│6.25U                   │1.5U │1.5U │1U │1U │1U │
 * └────┴────┴────┴────────────────────────┴─────┴─────┴───┴───┴───┘
 */

/* LAYOUT_custom
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
 * │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │ 16
 * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┴───┼───┤
 * │   │   │   │   │   │   │   │   │   │   │   │   │   │       │   │ 15
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┼───┤
 * │1.5U │   │   │   │   │   │   │   │   │   │   │   │   │1.5U │   │ 15
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┼───┤
 * │1.75U │   │   │   │   │   │   │   │   │   │   │   │2.25U   │   │ 14
 * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┼───┤
 * │2.25U   │   │   │   │   │   │   │   │   │   │   │1.75U │   │   │ 14
 * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┴┬──┴──┬───┼───┼───┤
 * │1.25│1.25│1.25│6.25U                   │1.5U │1.5U │1U │1U │1U │ 9
 * └────┴────┴────┴────────────────────────┴─────┴─────┴───┴───┴───┘
 */

#define LAYOUT_ansi( \
	K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, K014, K015, \
	K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112,       K114, K115, \
	K200,       K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, K215, \
	K300,       K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K313,       K315, \
	K400,       K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K413, K414, K415, \
	K500, K501,       K503,             K506,                   K510,       K512, K513, K514, K515  \
) { \
	{ K000,  K001,  K002,  K003,  K004,  K005,  K006,  K007,  K008,  K009,  K010,  K011,  K012,  K013,  K014,  K015 }, \
	{ K100,  K101,  K102,  K103,  K104,  K105,  K106,  K107,  K108,  K109,  K110,  K111,  K112,  KC_NO, K114,  K115 }, \
	{ K200,  KC_NO, K202,  K203,  K204,  K205,  K206,  K207,  K208,  K209,  K210,  K211,  K212,  K213,  K214,  K215 }, \
	{ K300,  KC_NO, K302,  K303,  K304,  K305,  K306,  K307,  K308,  K309,  K310,  K311,  K312,  K313,  KC_NO, K315 }, \
	{ K400,  KC_NO, K402,  K403,  K404,  K405,  K406,  K407,  K408,  K409,  K410,  K411,  KC_NO, K413,  K414,  K415 }, \
	{ K500,  K501,  KC_NO, K503,  KC_NO, KC_NO, K506,  KC_NO, KC_NO, KC_NO, K510,  KC_NO, K512,  K513,  K514,  K515 }  \
}



#define LAYOUT_iso( \
	K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, K014, K015, \
	K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112,       K114, K115, \
	K200,       K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K313, K215, \
	K300,       K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K214,       K315, \
	K400, K401, K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K413, K414, K415, \
	K500, K501,       K503,             K506,                   K510,       K512, K513, K514, K515  \
) { \
	{ K000,  K001,  K002,  K003,  K004,  K005,  K006,  K007,  K008,  K009,  K010,  K011,  K012,  K013,  K014,  K015 }, \
	{ K100,  K101,  K102,  K103,  K104,  K105,  K106,  K107,  K108,  K109,  K110,  K111,  K112,  KC_NO, K114,  K115 }, \
	{ K200,  KC_NO, K202,  K203,  K204,  K205,  K206,  K207,  K208,  K209,  K210,  K211,  K212,  K213,  K214,  K215 }, \
	{ K300,  KC_NO, K302,  K303,  K304,  K305,  K306,  K307,  K308,  K309,  K310,  K311,  K312,  K313,  KC_NO, K315 }, \
	{ K400,  K401,  K402,  K403,  K404,  K405,  K406,  K407,  K408,  K409,  K410,  K411,  KC_NO, K413,  K414,  K415 }, \
	{ K500,  K501,  KC_NO, K503,  KC_NO, KC_NO, K506,  KC_NO, KC_NO, KC_NO, K510,  KC_NO, K512,  K513,  K514,  K515 }  \
}

// LAYOUT for all possible switch positions on a KBD75 rev 2
#define LAYOUT_numpad( \
	K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, K014, K015, \
	K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, K115, \
	K200,       K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, K215, \
	K300,       K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K313,       K315, \
	K400, K401, K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K413, K414, K415, \
	K500, K501,       K503, K504,       K506,       K508, K509, K510, K511, K512, K513, K514, K515  \
) { \
	{ K000,  K001,  K002,  K003,  K004,  K005,  K006,  K007,  K008,  K009,  K010,  K011,  K012,  K013,  K014,  K015 }, \
	{ K100,  K101,  K102,  K103,  K104,  K105,  K106,  K107,  K108,  K109,  K110,  K111,  K112,  K113,  K114,  K115 }, \
	{ K200,  KC_NO, K202,  K203,  K204,  K205,  K206,  K207,  K208,  K209,  K210,  K211,  K212,  K213,  K214,  K215 }, \
	{ K300,  KC_NO, K302,  K303,  K304,  K305,  K306,  K307,  K308,  K309,  K310,  K311,  K312,  K313,  KC_NO, K315 }, \
	{ K400,  K401,  K402,  K403,  K404,  K405,  K406,  K407,  K408,  K409,  K410,  K411,  KC_NO, K413,  K414,  K415 }, \
	{ K500,  K501,  KC_NO, K503,  K504,  KC_NO, K506,  KC_NO, K508,  K509,  K510,  K511,  K512,  K513,  K514,  K515 }  \
}


