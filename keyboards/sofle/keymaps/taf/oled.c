/* Copyright 2020 Josef Adamcik
 * Modification for VIA support and RGB underglow by Jens Bonk-Wiltfang
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

// Sets up what the OLED screens display.
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef OLED_ENABLE

// Variables to keep track of typing speed and accuracy
static uint32_t start_time  = 0;
static uint32_t key_count   = 0;
static uint32_t error_count = 0;

void print_wpm(void) {
    // Increment key count for each keypress
    if (timer_elapsed(start_time) > 1000) { // Update every second
        oled_clear();
        oled_write_P(PSTR("Speed: \n"), false);

        uint32_t elapsed_time = timer_elapsed(start_time) / 1000; // Convert to seconds
        uint32_t typing_speed = key_count / elapsed_time;

        char speed_str[8];
        snprintf(speed_str, sizeof(speed_str), "%lu WPM", (unsigned long)typing_speed);
        oled_write(speed_str, false);

        float accuracy = 100.0 - (error_count * 100.0 / key_count);
        char  accuracy_str[8];
        snprintf(accuracy_str, sizeof(accuracy_str), "Acc: \n %.2f%%", accuracy);
        oled_write_P(PSTR("\n"), false);
        oled_write(accuracy_str, false);

        // Reset counts and start time for the next calculation
        key_count   = 0;
        error_count = 0;
        start_time  = timer_read();
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        // Increment key count on keypress
        key_count++;
    } else if (record->event.key.col != QK_LOCK) { // Use QK_LOCK instead of QK_NO_KEY
        // Increment error count on key release if a valid key was pressed
        error_count++;
    }
    return true;
}

static void print_status_narrow(void) {
    // Print current mode
    oled_write_P(PSTR("\n\n"), false);

    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_ln_P(PSTR("Qwrt"), false);
            break;
        case 1:
            oled_write_ln_P(PSTR("Clmk"), false);
            break;
        default:
            oled_write_P(PSTR("Mod\n"), false);
            break;
    }
    oled_write_P(PSTR("\n\n"), false);
    // Print current layer
    oled_write_ln_P(PSTR("LAYER"), false);
    switch (get_highest_layer(layer_state)) {
        case 0:
        case 1:
            oled_write_P(PSTR("Base\n"), false);
            break;
        case 2:
            oled_write_P(PSTR("Raise"), false);
            break;
        case 3:
            oled_write_P(PSTR("Lower"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undef"), false);
    }
    oled_write_P(PSTR("\n\n"), false);
    led_t led_usb_state = host_keyboard_led_state();
    oled_write_ln_P(PSTR("CPSLK"), led_usb_state.caps_lock);
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    start_time = timer_read();

    return OLED_ROTATION_270;
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        print_status_narrow();
    } else {
        print_wpm();
    }
    return false;
}

#endif
