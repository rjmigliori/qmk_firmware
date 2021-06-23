/* Copyright 2019 Ryota Goto
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
#include QMK_KEYBOARD_H
enum custom_keycodes {
	TAB_LEFT = SAFE_RANGE,
	TAB_RIGHT,
    WIN_RIGHT, 
    WIN_LEFT,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT( /* Base */
    
	KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,        KC_F5,   KC_F6,   KC_F7,   KC_F8,       KC_F9,   KC_F10,  KC_F11,  KC_F12,           KC_PSCR, KC_SLCK, KC_PAUS, \
	KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,           KC_INS,  KC_HOME, KC_PGUP, \
	KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,          KC_BSLS,          KC_DEL,  KC_END,  KC_PGDN, \
	KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,                   KC_ENT,                                      \
	KC_LSFT, KC_NO,   KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_DEL,                    KC_UP,            \
	KC_LCTL, KC_LGUI, KC_LALT,                           KC_SPC,                                       KC_RALT, KC_RGUI, MO(1),  KC_RCTL,          KC_LEFT, KC_DOWN, KC_RGHT  \
  ),
  [1] = LAYOUT( /* Layer 1 */
    
	KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,        KC_F5,   KC_F6,   KC_F7,   KC_F8,       KC_F9,   KC_F10,  KC_F11,  KC_F12,           KC_PSCR, KC_SLCK, RESET, \
	KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,           KC_INS,  KC_HOME, KC_PGUP, \
	KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,          KC_BSLS,          KC_DEL,  KC_END,  KC_PGDN, \
	KC_CAPS, TAB_LEFT,KC_S,TAB_RIGHT,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,                   KC_ENT,                                      \
	KC_LSFT, KC_NO,RGB_TOG,RGB_HUD,RGB_HUI,RGB_SAD,RGB_SAI,RGB_VAD,RGB_VAI,RGB_MODE_PLAIN, RGB_MODE_RAINBOW,  KC_SLSH,          KC_RSFT, KC_DEL,                    KC_UP,            \
	KC_LCTL, KC_LGUI, KC_LALT,                           KC_SPC,                                       KC_RALT, KC_RGUI, KC_TRNS,  KC_RCTL,          KC_LEFT, KC_DOWN, KC_RGHT  \
  ),  
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode){
		case TAB_LEFT:
			if (record->event.pressed){
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				SEND_STRING(SS_TAP(X_TAB));			
			} else {
				//When keycode TAB_LEFT is released 
				unregister_code(KC_LCTL);
				unregister_code(KC_LSFT);
			}
			break;					
		case TAB_RIGHT:
			if (record->event.pressed){
				register_code(KC_LCTL);
				SEND_STRING(SS_TAP(X_TAB));			
			} else {
				//When keycode TAB_RIGHT is released 
				unregister_code(KC_LCTL);
			}
			break;	
	    case WIN_RIGHT:
	      if (record->event.pressed){
	        register_code(KC_LGUI);
	        register_code(KC_LCTL);
	        register_code(KC_RGHT);        
	      } else {
	        unregister_code(KC_LGUI);
	        unregister_code(KC_LCTL);
	        unregister_code(KC_RGHT);    
	      }
	      break;
	    case WIN_LEFT:
	      if (record->event.pressed){
	        register_code(KC_LGUI);
	        register_code(KC_LCTL);
	        register_code(KC_LEFT);        
	      } else {
	        unregister_code(KC_LGUI);
	        unregister_code(KC_LCTL);
	        unregister_code(KC_LEFT);    
	      }
	      break;   						
	}
  return true;
}

void matrix_init_user(void) {

}

void matrix_scan_user(void) {

}

void led_set_user(uint8_t usb_led) {

}
