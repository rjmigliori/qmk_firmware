/* (C) 10t8or, 2o2o
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

#pragma once

#include "config_common.h"

#undef  PRODUCT
#define PRODUCT      10t_f62

#define FORCE_NKRO
#define TAPPING_TERM 150                // default is 200
#define PERMISSIVE_HOLD                 // for fast typers: makes tap and hold keys trigger the hold if another key is pressed before releasingmakes tap and hold keys trigger the hold if another key is pressed before releasing
#define IGNORE_MOD_TAP_INTERRUPT        // enables rolling combos
#undef TAPPING_FORCE_HOLD               // se lo setti il doppio tap di un key non attiva l'autorepeat 
// #define #GRAVE_ESC_GUI_OVERRIDE      // non dovrebbe servire, ma se usi KC_GESC almeno GUI+ESC non fa nulla (normalmente fa tilde)
#undef NO_ACTION_ONESHOT                // con questo poi non va OSL() !!
#define ONESHOT_TAP_TOGGLE 999
// some speed shit
// #define NO_ACTION_MACRO
// #define NO_ACTION_FUNCTION

/* Set 0 if debouncing isn't needed */ 
//#define DEBOUNCE 5                     


