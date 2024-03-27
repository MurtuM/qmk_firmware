#include QMK_KEYBOARD_H

#include <stdio.h>

#include "images.h"
#include "helpers.h"

#define LETTERS 0
#define GAMING 1
#define NUMBERS 2
#define F_ROW 3
#define NAV 4

enum custom_keycodes {
    _MACRO_REC = SAFE_RANGE,
    _MACRO_PRINT,
    _FAST_STEP_UP,
    _FAST_STEP_DOWN,
    _TILDE_NO_SHIT,
    _ROOF_NO_SHIT
};

#define MACRO_MAX_SIZE 128
static uint16_t s_macro[MACRO_MAX_SIZE];
static int s_macro_cursor = 0;
static bool s_macro_rec = false;
static uint16_t s_last_pressed = 0;
static int s_macro_restart_on_input = false;

    /*
    * LSFT(KC_NUHS)
    ^ LSFT(KC_RBRC)
    ' KC_BSLS
    ~ RALT(KC_RBRC)

    1 2 3 4 5 6 7 8 9 0 +
    ! " # ¤ % & / ( ) = ?
    | < > $ @ ¤ { [ ] } \
    */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LETTERS] = LAYOUT_split_3x6_3(
	KC_TAB,     KC_Q,   KC_W,   KC_E,   KC_R,   KC_T,                     KC_Y, KC_U, KC_I,     KC_O,   KC_P,       KC_LBRC,
	MO(NAV),    KC_A,   KC_S,   KC_D,   KC_F,   KC_G,                     KC_H, KC_J, KC_K,     KC_L,   KC_SCLN,    KC_QUOT,
	KC_LSFT,    KC_Z,   KC_X,   KC_C,   KC_V,   KC_B,                     KC_N, KC_M, KC_COMM,  KC_DOT, KC_SLSH,    KC_LALT,
	                          KC_LCTL, MO(NUMBERS), KC_LSFT,       KC_BSPC, KC_SPC, MO(F_ROW)),

    [GAMING] = LAYOUT_split_3x6_3(
	KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T,                      KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC,
	MO(NAV), KC_A, KC_S, KC_D, KC_F, KC_G,                     KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
	KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B,                     KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_LALT,
	                KC_LCTL, MO(NUMBERS), KC_SPC,     KC_BSPC, KC_SPC, MO(F_ROW)),

    [NUMBERS] = LAYOUT_split_3x6_3(
	KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5,                                              KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS,
	KC_ESC, RSFT(KC_1), RSFT(KC_2), RSFT(KC_3), LSFT(KC_NUHS), RSFT(KC_5),             LSFT(KC_6), LSFT(KC_7), LSFT(KC_8), LSFT(KC_9), LSFT(KC_0), LSFT(KC_MINS),
	_______, RALT(KC_NUBS), KC_NUBS, LSFT(KC_NUBS), RALT(KC_4), RALT(KC_2),            RSFT(KC_4), RALT(KC_7), RALT(KC_8), RALT(KC_9), RALT(KC_0), RALT(KC_MINS),
	                                            _______, _______, _______,    _______, _______, _______),

    [NAV] = LAYOUT_split_3x6_3(
	_______, XXXXXXX, KC_TILD, KC_CIRC, _MACRO_REC, _MACRO_PRINT,               _FAST_STEP_UP, KC_HOME, KC_UP,   KC_END,  KC_PGUP, _TILDE_NO_SHIT  /*RALT(KC_RBRC)~*/,
	_______, XXXXXXX, KC_LCTL, KC_LSFT, KC_LALT, KC_LGUI,                       _FAST_STEP_DOWN, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN,  KC_BSLS/*'*/,
	_______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                       KC_F3, _______, _______, _______, _______, _ROOF_NO_SHIT /*LSFT(KC_RBRC)^*/,
                                        _______, _______, _______,      KC_DEL, KC_ENT, XXXXXXX),

    [F_ROW] = LAYOUT_split_3x6_3(
	LALT(KC_F4), KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,                               KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11,
	KC_LGUI, LGUI(KC_1), LGUI(KC_2), LGUI(KC_3), LGUI(KC_4), LGUI(KC_5),          LGUI(KC_6), LGUI(KC_7), LGUI(KC_8), LGUI(KC_9), LGUI(KC_0), KC_F12,
	RGB_TOG, RGB_MOD, RGB_HUI, RGB_SAI, RGB_VAI, RGB_SPI,                         TG(GAMING), KC_MPLY, KC_MPRV, KC_MNXT, KC_PSCR, KC_PAUS,
                                    _______, KC_CAPS, _______,         QK_BOOT, _______, _______)};

                                    
void keyboard_pre_init_user(void) {
  // Set our LED pin as output
  setPinOutput(24);
  // Turn the LED off
  // (Due to technical reasons, high is off and low is on)
  writePinHigh(24);
}

static char to_char(int Keycode)
{
    // return unicodemap_index(Keycode);

    switch(Keycode)
    {
        case 44: return ' ';
        case 41: return 'e';
        case 39: return '0';
        default: 
        if (Keycode >= 30 && Keycode < 39)
            return Keycode + 19;
        return Keycode + 61;
    }
}

bool oled_task_user(void) 
{
    if (is_keyboard_master())
    {
        oled_clear();

        if (s_macro_rec)
        {
            // oled_clear();
            oled_write_ln_P(PSTR("macro"), false);

            for (size_t i = 0; i < s_macro_cursor; i++)
                oled_write_char(to_char(s_macro[i]), false);

            return false;
        }

        if (layer_state_is(GAMING))
        {
            oled_write_raw_P(s_image_gaming, sizeof(s_image_gaming));
            return false;
        }

        oled_write_raw_P(s_image_dmw, sizeof(s_image_dmw));
        oled_set_cursor(0, 5);

        // oled_write_P(PSTR("corne"), false);
        // render_space();

        render_layer_state(get_highest_layer(layer_state | default_layer_state));
        render_space();
        render_mod_status_gui_alt(get_mods()|get_oneshot_mods());
        render_mod_status_ctrl_shift(get_mods()|get_oneshot_mods());
        render_kb_LED_state();

        return false;
    }

    // Slave OLED 
    
    oled_write_raw_P(s_image_dmw_big, sizeof(s_image_dmw_big));

    return false;
}

static void print_murtum_macro(void)
{
    s_macro_rec = false;
    writePinHigh(24);

    for (int i = 0; i < s_macro_cursor; i++)
        tap_code16(s_macro[i]);
}

static void tap_with_macro_support(uint16_t keycode, bool tap)
{
    if (tap)
        tap_code16(keycode);

    if ((keycode < QK_MODS_MAX || keycode > SAFE_RANGE) && s_macro_rec && s_macro_cursor < MACRO_MAX_SIZE)
    {
        if (s_macro_restart_on_input)
            s_macro_cursor = 0;

        s_macro_restart_on_input = false;

        if (get_mods() & MOD_MASK_ALT)
            keycode = LALT(keycode);
        if (get_mods() & MOD_MASK_CTRL)
            keycode = LCTL(keycode);
        if (get_mods() & MOD_MASK_SHIFT)
            keycode = LSFT(keycode);
        if (get_mods() & MOD_MASK_GUI)
            keycode = LGUI(keycode);

        s_macro[s_macro_cursor++] = keycode;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    if (keycode == _FAST_STEP_UP && record->event.pressed)
    {
        for (int i = 0; i < 5; i++)
            tap_with_macro_support(KC_UP, true);
        return false;
    }
    else if (keycode == _FAST_STEP_DOWN && record->event.pressed)
    {
        for (int i = 0; i < 5; i++)
            tap_with_macro_support(KC_DOWN, true);
        return false;
    }

    if (!record->event.pressed)
    {
        s_last_pressed = 0;
        return true;
    }
    s_last_pressed = keycode;

    if (keycode == _MACRO_REC)
    {
        if (!s_macro_rec)
        {
            s_macro_restart_on_input = true;
            writePinLow(24);    
        }
        else
            writePinHigh(24);

        s_macro_rec = !s_macro_rec;

        return false;
    }
    else if (keycode == _MACRO_PRINT)
    {
        print_murtum_macro();
        return false;
    }
    else
        tap_with_macro_support(keycode, false);

    if (keycode == _TILDE_NO_SHIT)
    {
        tap_with_macro_support(RALT(KC_RBRC), true);
        tap_with_macro_support(KC_SPC, true);
        return false;
    }
    else if (keycode == _ROOF_NO_SHIT)
    {
        tap_with_macro_support(LSFT(KC_RBRC), true);
        tap_with_macro_support(KC_SPC, true);
        return false;
    }

    return true;
}