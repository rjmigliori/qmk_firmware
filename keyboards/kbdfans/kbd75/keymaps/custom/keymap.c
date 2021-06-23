#include QMK_KEYBOARD_H

enum custom_keycodes {
  TAB_LEFT = SAFE_RANGE,
  TAB_RIGHT,
  WIN_RIGHT, 
  WIN_LEFT,
};

/* LAYOUT
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
 * │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │
 * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┴───┼───┤
 * │   │   │   │   │   │   │   │   │   │   │   │   │   │       │   │
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┼───┤
 * │1.5U │   │   │   │   │   │   │   │   │   │   │   │   │1.5U │   │
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┼───┤
 * │1.75U │   │   │   │   │   │   │   │   │   │   │   │2.25U   │   │
 * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┼───┤
 * │2.25U   │   │   │   │   │   │   │   │   │   │   │1.75U │   │   │
 * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┴┬──┴──┬───┼───┼───┤
 * │1.25│1.25│1.25│6.25U                   │1.5U │1.5U │1U │1U │1U │
 * └────┴────┴────┴────────────────────────┴─────┴─────┴───┴───┴───┘
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [0] = LAYOUT_ansi(
    KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  WIN_LEFT,  WIN_RIGHT,
    KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,             KC_BSPC,  KC_HOME,
    KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,             KC_END,
    KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,                      KC_ENT,    KC_DEL,
    KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,            KC_UP,      MO(1),
    KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 TAB_LEFT,           TAB_RIGHT,  KC_LEFT,  KC_DOWN,  KC_RGHT
  ),

  [1] = LAYOUT_ansi(
    KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  WIN_LEFT,  WIN_RIGHT,
    KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,             KC_BSPC,  KC_HOME,
    KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,             KC_END,
    KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,                      KC_ENT,    KC_DEL,
    KC_LSFT,         RGB_TOG,RGB_HUD,RGB_HUI,RGB_SAD,RGB_SAI,RGB_VAD,RGB_VAI,RGB_MODE_PLAIN, RGB_MODE_RAINBOW,    KC_SLSH,  KC_RSFT,            KC_UP,      KC_TRNS,
    KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT,            KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT
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
    //Prolly not the best way of doing this but it works so whatever 
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
