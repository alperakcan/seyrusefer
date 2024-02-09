
#include <stdint.h>

#include "hid.h"
#include "settings.h"

const char * seyrusefer_settings_mode_to_string (int mode)
{
        if (mode == SEYRUSEFER_SETTINGS_MODE_1) return "MODE_1";
        if (mode == SEYRUSEFER_SETTINGS_MODE_2) return "MODE_2";
        if (mode == SEYRUSEFER_SETTINGS_MODE_3) return "MODE_3";
        if (mode == SEYRUSEFER_SETTINGS_MODE_4) return "MODE_4";
        if (mode == SEYRUSEFER_SETTINGS_MODE_5) return "MODE_5";
        return "invalid";
}

const char * seyrusefer_settings_button_to_string (int button)
{
        if (button == SEYRUSEFER_SETTINGS_BUTTON_1) return "BUTTON_1";
        if (button == SEYRUSEFER_SETTINGS_BUTTON_2) return "BUTTON_2";
        if (button == SEYRUSEFER_SETTINGS_BUTTON_3) return "BUTTON_3";
        if (button == SEYRUSEFER_SETTINGS_BUTTON_4) return "BUTTON_4";
        if (button == SEYRUSEFER_SETTINGS_BUTTON_5) return "BUTTON_5";
        return "invalid";
}
const char * seyrusefer_settings_key_to_string (int key)
{
        if (key == SEYRUSEFER_HID_KEY_RESERVED) return "KEY_RESERVED";
        if (key == SEYRUSEFER_HID_KEY_A) return "KEY_A";
        if (key == SEYRUSEFER_HID_KEY_B) return "KEY_B";
        if (key == SEYRUSEFER_HID_KEY_C) return "KEY_C";
        if (key == SEYRUSEFER_HID_KEY_D) return "KEY_D";
        if (key == SEYRUSEFER_HID_KEY_E) return "KEY_E";
        if (key == SEYRUSEFER_HID_KEY_F) return "KEY_F";
        if (key == SEYRUSEFER_HID_KEY_G) return "KEY_G";
        if (key == SEYRUSEFER_HID_KEY_H) return "KEY_H";
        if (key == SEYRUSEFER_HID_KEY_I) return "KEY_I";
        if (key == SEYRUSEFER_HID_KEY_J) return "KEY_J";
        if (key == SEYRUSEFER_HID_KEY_K) return "KEY_K";
        if (key == SEYRUSEFER_HID_KEY_L) return "KEY_L";
        if (key == SEYRUSEFER_HID_KEY_M) return "KEY_M";
        if (key == SEYRUSEFER_HID_KEY_N) return "KEY_N";
        if (key == SEYRUSEFER_HID_KEY_O) return "KEY_O";
        if (key == SEYRUSEFER_HID_KEY_P) return "KEY_P";
        if (key == SEYRUSEFER_HID_KEY_Q) return "KEY_Q";
        if (key == SEYRUSEFER_HID_KEY_R) return "KEY_R";
        if (key == SEYRUSEFER_HID_KEY_S) return "KEY_S";
        if (key == SEYRUSEFER_HID_KEY_T) return "KEY_T";
        if (key == SEYRUSEFER_HID_KEY_U) return "KEY_U";
        if (key == SEYRUSEFER_HID_KEY_V) return "KEY_V";
        if (key == SEYRUSEFER_HID_KEY_W) return "KEY_W";
        if (key == SEYRUSEFER_HID_KEY_X) return "KEY_X";
        if (key == SEYRUSEFER_HID_KEY_Y) return "KEY_Y";
        if (key == SEYRUSEFER_HID_KEY_Z) return "KEY_Z";
        if (key == SEYRUSEFER_HID_KEY_1) return "KEY_1";
        if (key == SEYRUSEFER_HID_KEY_2) return "KEY_2";
        if (key == SEYRUSEFER_HID_KEY_3) return "KEY_3";
        if (key == SEYRUSEFER_HID_KEY_4) return "KEY_4";
        if (key == SEYRUSEFER_HID_KEY_5) return "KEY_5";
        if (key == SEYRUSEFER_HID_KEY_6) return "KEY_6";
        if (key == SEYRUSEFER_HID_KEY_7) return "KEY_7";
        if (key == SEYRUSEFER_HID_KEY_8) return "KEY_8";
        if (key == SEYRUSEFER_HID_KEY_9) return "KEY_9";
        if (key == SEYRUSEFER_HID_KEY_0) return "KEY_0";
        if (key == SEYRUSEFER_HID_KEY_RETURN) return "KEY_RETURN";
        if (key == SEYRUSEFER_HID_KEY_ESCAPE) return "KEY_ESCAPE";
        if (key == SEYRUSEFER_HID_KEY_DELETE) return "KEY_DELETE";
        if (key == SEYRUSEFER_HID_KEY_TAB) return "KEY_TAB";
        if (key == SEYRUSEFER_HID_KEY_SPACEBAR) return "KEY_SPACEBAR";
        if (key == SEYRUSEFER_HID_KEY_MINUS) return "KEY_MINUS";
        if (key == SEYRUSEFER_HID_KEY_EQUAL) return "KEY_EQUAL";
        if (key == SEYRUSEFER_HID_KEY_LEFT_BRKT) return "KEY_LEFT_BRKT";
        if (key == SEYRUSEFER_HID_KEY_RIGHT_BRKT) return "KEY_RIGHT_BRKT";
        if (key == SEYRUSEFER_HID_KEY_BACK_SLASH) return "KEY_BACK_SLASH";
        if (key == SEYRUSEFER_HID_KEY_SEMI_COLON) return "KEY_SEMI_COLON";
        if (key == SEYRUSEFER_HID_KEY_SGL_QUOTE) return "KEY_SGL_QUOTE";
        if (key == SEYRUSEFER_HID_KEY_GRV_ACCENT) return "KEY_GRV_ACCENT";
        if (key == SEYRUSEFER_HID_KEY_COMMA) return "KEY_COMMA";
        if (key == SEYRUSEFER_HID_KEY_DOT) return "KEY_DOT";
        if (key == SEYRUSEFER_HID_KEY_FWD_SLASH) return "KEY_FWD_SLASH";
        if (key == SEYRUSEFER_HID_KEY_CAPS_LOCK) return "KEY_CAPS_LOCK";
        if (key == SEYRUSEFER_HID_KEY_F1) return "KEY_F1";
        if (key == SEYRUSEFER_HID_KEY_F2) return "KEY_F2";
        if (key == SEYRUSEFER_HID_KEY_F3) return "KEY_F3";
        if (key == SEYRUSEFER_HID_KEY_F4) return "KEY_F4";
        if (key == SEYRUSEFER_HID_KEY_F5) return "KEY_F5";
        if (key == SEYRUSEFER_HID_KEY_F6) return "KEY_F6";
        if (key == SEYRUSEFER_HID_KEY_F7) return "KEY_F7";
        if (key == SEYRUSEFER_HID_KEY_F8) return "KEY_F8";
        if (key == SEYRUSEFER_HID_KEY_F9) return "KEY_F9";
        if (key == SEYRUSEFER_HID_KEY_F10) return "KEY_F10";
        if (key == SEYRUSEFER_HID_KEY_F11) return "KEY_F11";
        if (key == SEYRUSEFER_HID_KEY_F12) return "KEY_F12";
        if (key == SEYRUSEFER_HID_KEY_PRNT_SCREEN) return "KEY_PRNT_SCREEN";
        if (key == SEYRUSEFER_HID_KEY_SCROLL_LOCK) return "KEY_SCROLL_LOCK";
        if (key == SEYRUSEFER_HID_KEY_PAUSE) return "KEY_PAUSE";
        if (key == SEYRUSEFER_HID_KEY_INSERT) return "KEY_INSERT";
        if (key == SEYRUSEFER_HID_KEY_HOME) return "KEY_HOME";
        if (key == SEYRUSEFER_HID_KEY_PAGE_UP) return "KEY_PAGE_UP";
        if (key == SEYRUSEFER_HID_KEY_DELETE_FWD) return "KEY_DELETE_FWD";
        if (key == SEYRUSEFER_HID_KEY_END) return "KEY_END";
        if (key == SEYRUSEFER_HID_KEY_PAGE_DOWN) return "KEY_PAGE_DOWN";
        if (key == SEYRUSEFER_HID_KEY_RIGHT_ARROW) return "KEY_RIGHT_ARROW";
        if (key == SEYRUSEFER_HID_KEY_LEFT_ARROW) return "KEY_LEFT_ARROW";
        if (key == SEYRUSEFER_HID_KEY_DOWN_ARROW) return "KEY_DOWN_ARROW";
        if (key == SEYRUSEFER_HID_KEY_UP_ARROW) return "KEY_UP_ARROW";
        if (key == SEYRUSEFER_HID_KEY_NUM_LOCK) return "KEY_NUM_LOCK";
        if (key == SEYRUSEFER_HID_KEY_DIVIDE) return "KEY_DIVIDE";
        if (key == SEYRUSEFER_HID_KEY_MULTIPLY) return "KEY_MULTIPLY";
        if (key == SEYRUSEFER_HID_KEY_SUBTRACT) return "KEY_SUBTRACT";
        if (key == SEYRUSEFER_HID_KEY_ADD) return "KEY_ADD";
        if (key == SEYRUSEFER_HID_KEY_ENTER) return "KEY_ENTER";
        if (key == SEYRUSEFER_HID_KEYPAD_1) return "KEYPAD_1";
        if (key == SEYRUSEFER_HID_KEYPAD_2) return "KEYPAD_2";
        if (key == SEYRUSEFER_HID_KEYPAD_3) return "KEYPAD_3";
        if (key == SEYRUSEFER_HID_KEYPAD_4) return "KEYPAD_4";
        if (key == SEYRUSEFER_HID_KEYPAD_5) return "KEYPAD_5";
        if (key == SEYRUSEFER_HID_KEYPAD_6) return "KEYPAD_6";
        if (key == SEYRUSEFER_HID_KEYPAD_7) return "KEYPAD_7";
        if (key == SEYRUSEFER_HID_KEYPAD_8) return "KEYPAD_8";
        if (key == SEYRUSEFER_HID_KEYPAD_9) return "KEYPAD_9";
        if (key == SEYRUSEFER_HID_KEYPAD_0) return "KEYPAD_0";
        if (key == SEYRUSEFER_HID_KEYPAD_DOT) return "KEYPAD_DOT";
        if (key == SEYRUSEFER_HID_KEY_MUTE) return "KEY_MUTE";
        if (key == SEYRUSEFER_HID_KEY_VOLUME_UP) return "KEY_VOLUME_UP";
        if (key == SEYRUSEFER_HID_KEY_VOLUME_DOWN) return "KEY_VOLUME_DOWN";
        if (key == SEYRUSEFER_HID_KEY_LEFT_CTRL) return "KEY_LEFT_CTRL";
        if (key == SEYRUSEFER_HID_KEY_LEFT_SHIFT) return "KEY_LEFT_SHIFT";
        if (key == SEYRUSEFER_HID_KEY_LEFT_ALT) return "KEY_LEFT_ALT";
        if (key == SEYRUSEFER_HID_KEY_LEFT_GUI) return "KEY_LEFT_GUI";
        if (key == SEYRUSEFER_HID_KEY_RIGHT_CTRL) return "KEY_RIGHT_CTRL";
        if (key == SEYRUSEFER_HID_KEY_RIGHT_SHIFT) return "KEY_RIGHT_SHIFT";
        if (key == SEYRUSEFER_HID_KEY_RIGHT_ALT) return "KEY_RIGHT_ALT";
        if (key == SEYRUSEFER_HID_KEY_RIGHT_GUI) return "KEY_RIGHT_GUI";
        if (key == SEYRUSEFER_HID_MOUSE_LEFT) return "MOUSE_LEFT";
        if (key == SEYRUSEFER_HID_MOUSE_MIDDLE) return "MOUSE_MIDDLE";
        if (key == SEYRUSEFER_HID_MOUSE_RIGHT) return "MOUSE_RIGHT";
        if (key == SEYRUSEFER_HID_CONSUMER_POWER) return "CONSUMER_POWER";
        if (key == SEYRUSEFER_HID_CONSUMER_RESET) return "CONSUMER_RESET";
        if (key == SEYRUSEFER_HID_CONSUMER_SLEEP) return "CONSUMER_SLEEP";
        if (key == SEYRUSEFER_HID_CONSUMER_MENU) return "CONSUMER_MENU";
        if (key == SEYRUSEFER_HID_CONSUMER_SELECTION) return "CONSUMER_SELECTION";
        if (key == SEYRUSEFER_HID_CONSUMER_ASSIGN_SEL) return "CONSUMER_ASSIGN_SEL";
        if (key == SEYRUSEFER_HID_CONSUMER_MODE_STEP) return "CONSUMER_MODE_STEP";
        if (key == SEYRUSEFER_HID_CONSUMER_RECALL_LAST) return "CONSUMER_RECALL_LAST";
        if (key == SEYRUSEFER_HID_CONSUMER_QUIT) return "CONSUMER_QUIT";
        if (key == SEYRUSEFER_HID_CONSUMER_HELP) return "CONSUMER_HELP";
        if (key == SEYRUSEFER_HID_CONSUMER_CHANNEL_UP) return "CONSUMER_CHANNEL_UP";
        if (key == SEYRUSEFER_HID_CONSUMER_CHANNEL_DOWN) return "CONSUMER_CHANNEL_DOWN";
        if (key == SEYRUSEFER_HID_CONSUMER_PLAY) return "CONSUMER_PLAY";
        if (key == SEYRUSEFER_HID_CONSUMER_PAUSE) return "CONSUMER_PAUSE";
        if (key == SEYRUSEFER_HID_CONSUMER_RECORD) return "CONSUMER_RECORD";
        if (key == SEYRUSEFER_HID_CONSUMER_FAST_FORWARD) return "CONSUMER_FAST_FORWARD";
        if (key == SEYRUSEFER_HID_CONSUMER_REWIND) return "CONSUMER_REWIND";
        if (key == SEYRUSEFER_HID_CONSUMER_SCAN_NEXT_TRK) return "CONSUMER_SCAN_NEXT_TRK";
        if (key == SEYRUSEFER_HID_CONSUMER_SCAN_PREV_TRK) return "CONSUMER_SCAN_PREV_TRK";
        if (key == SEYRUSEFER_HID_CONSUMER_STOP) return "CONSUMER_STOP";
        if (key == SEYRUSEFER_HID_CONSUMER_EJECT) return "CONSUMER_EJECT";
        if (key == SEYRUSEFER_HID_CONSUMER_RANDOM_PLAY) return "CONSUMER_RANDOM_PLAY";
        if (key == SEYRUSEFER_HID_CONSUMER_SELECT_DISC) return "CONSUMER_SELECT_DISC";
        if (key == SEYRUSEFER_HID_CONSUMER_ENTER_DISC) return "CONSUMER_ENTER_DISC";
        if (key == SEYRUSEFER_HID_CONSUMER_REPEAT) return "CONSUMER_REPEAT";
        if (key == SEYRUSEFER_HID_CONSUMER_STOP_EJECT) return "CONSUMER_STOP_EJECT";
        if (key == SEYRUSEFER_HID_CONSUMER_PLAY_PAUSE) return "CONSUMER_PLAY_PAUSE";
        if (key == SEYRUSEFER_HID_CONSUMER_PLAY_SKIP) return "CONSUMER_PLAY_SKIP";
        if (key == SEYRUSEFER_HID_CONSUMER_VOLUME) return "CONSUMER_VOLUME";
        if (key == SEYRUSEFER_HID_CONSUMER_BALANCE) return "CONSUMER_BALANCE";
        if (key == SEYRUSEFER_HID_CONSUMER_MUTE) return "CONSUMER_MUTE";
        if (key == SEYRUSEFER_HID_CONSUMER_BASS) return "CONSUMER_BASS";
        if (key == SEYRUSEFER_HID_CONSUMER_VOLUME_UP) return "CONSUMER_VOLUME_UP";
        if (key == SEYRUSEFER_HID_CONSUMER_VOLUME_DOWN) return "CONSUMER_VOLUME_DOWN";
        return "invalid";
}
