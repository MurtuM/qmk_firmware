#include QMK_KEYBOARD_H

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
static int s_macro_pulse_count = 0;

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



static void print_murtum_macro(void)
{
    s_macro_rec = false;
    for (int i = 0; i < s_macro_cursor; i++)
        tap_code16(s_macro[i]);
}

static void tap_with_macro_support(uint16_t keycode, bool tap)
{
    if (tap)
        tap_code16(keycode);

    if ((keycode < QK_MODS_MAX || keycode > SAFE_RANGE) && s_macro_rec && s_macro_cursor < MACRO_MAX_SIZE)
    {
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
            s_macro_cursor = 0;
            s_macro_pulse_count = 0;
        }
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
};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max)
{
    if (layer_state_is(GAMING))
        rgb_matrix_set_color(38, 10, 10, 60);

    if (s_macro_rec)
    {
        int start = 21;
        int num_indicators = 39 - start;
        float capacity_fac = s_macro_cursor / (float)MACRO_MAX_SIZE;

        for (int i = 0; i < (int)(capacity_fac * num_indicators); i++)
        {
            rgb_matrix_set_color(start + i,
                capacity_fac * 100,
                70 * (1.f - capacity_fac),
                50 * (1.f - capacity_fac));
        }

        s_macro_pulse_count++;
        int r1 = 10 + (1.f + sin((s_macro_pulse_count / 500.f) * 6.28f)) * 60;
        int r2 = 10 + (1.f + sin(((s_macro_pulse_count + 150) / 500.f) * 6.28f)) * 60;
        int r3 = 10 + (1.f + sin(((s_macro_pulse_count + 300) / 500.f) * 6.28f)) * 60;

        rgb_matrix_set_color(39, r1, 0, 0);
        rgb_matrix_set_color(40, r2, 0, 0);
        rgb_matrix_set_color(41, r3, 0, 0);
    }

    return true;
}
