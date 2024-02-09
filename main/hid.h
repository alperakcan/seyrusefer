
struct seyrusefer_config;
struct seyrusefer_hid;

enum {
        SEYRUSEFER_HID_KEY_RESERVED,
        SEYRUSEFER_HID_KEY_A,
        SEYRUSEFER_HID_KEY_B,
        SEYRUSEFER_HID_KEY_C,
        SEYRUSEFER_HID_KEY_D,
        SEYRUSEFER_HID_KEY_E,
        SEYRUSEFER_HID_KEY_F,
        SEYRUSEFER_HID_KEY_G,
        SEYRUSEFER_HID_KEY_H,
        SEYRUSEFER_HID_KEY_I,
        SEYRUSEFER_HID_KEY_J,
        SEYRUSEFER_HID_KEY_K,
        SEYRUSEFER_HID_KEY_L,
        SEYRUSEFER_HID_KEY_M,
        SEYRUSEFER_HID_KEY_N,
        SEYRUSEFER_HID_KEY_O,
        SEYRUSEFER_HID_KEY_P,
        SEYRUSEFER_HID_KEY_Q,
        SEYRUSEFER_HID_KEY_R,
        SEYRUSEFER_HID_KEY_S,
        SEYRUSEFER_HID_KEY_T,
        SEYRUSEFER_HID_KEY_U,
        SEYRUSEFER_HID_KEY_V,
        SEYRUSEFER_HID_KEY_W,
        SEYRUSEFER_HID_KEY_X,
        SEYRUSEFER_HID_KEY_Y,
        SEYRUSEFER_HID_KEY_Z,
        SEYRUSEFER_HID_KEY_1,
        SEYRUSEFER_HID_KEY_2,
        SEYRUSEFER_HID_KEY_3,
        SEYRUSEFER_HID_KEY_4,
        SEYRUSEFER_HID_KEY_5,
        SEYRUSEFER_HID_KEY_6,
        SEYRUSEFER_HID_KEY_7,
        SEYRUSEFER_HID_KEY_8,
        SEYRUSEFER_HID_KEY_9,
        SEYRUSEFER_HID_KEY_0,
        SEYRUSEFER_HID_KEY_RETURN,
        SEYRUSEFER_HID_KEY_ESCAPE,
        SEYRUSEFER_HID_KEY_DELETE,
        SEYRUSEFER_HID_KEY_TAB,
        SEYRUSEFER_HID_KEY_SPACEBAR,
        SEYRUSEFER_HID_KEY_MINUS,
        SEYRUSEFER_HID_KEY_EQUAL,
        SEYRUSEFER_HID_KEY_LEFT_BRKT,
        SEYRUSEFER_HID_KEY_RIGHT_BRKT,
        SEYRUSEFER_HID_KEY_BACK_SLASH,
        SEYRUSEFER_HID_KEY_SEMI_COLON,
        SEYRUSEFER_HID_KEY_SGL_QUOTE,
        SEYRUSEFER_HID_KEY_GRV_ACCENT,
        SEYRUSEFER_HID_KEY_COMMA,
        SEYRUSEFER_HID_KEY_DOT,
        SEYRUSEFER_HID_KEY_FWD_SLASH,
        SEYRUSEFER_HID_KEY_CAPS_LOCK,
        SEYRUSEFER_HID_KEY_F1,
        SEYRUSEFER_HID_KEY_F2,
        SEYRUSEFER_HID_KEY_F3,
        SEYRUSEFER_HID_KEY_F4,
        SEYRUSEFER_HID_KEY_F5,
        SEYRUSEFER_HID_KEY_F6,
        SEYRUSEFER_HID_KEY_F7,
        SEYRUSEFER_HID_KEY_F8,
        SEYRUSEFER_HID_KEY_F9,
        SEYRUSEFER_HID_KEY_F10,
        SEYRUSEFER_HID_KEY_F11,
        SEYRUSEFER_HID_KEY_F12,
        SEYRUSEFER_HID_KEY_PRNT_SCREEN,
        SEYRUSEFER_HID_KEY_SCROLL_LOCK,
        SEYRUSEFER_HID_KEY_PAUSE,
        SEYRUSEFER_HID_KEY_INSERT,
        SEYRUSEFER_HID_KEY_HOME,
        SEYRUSEFER_HID_KEY_PAGE_UP,
        SEYRUSEFER_HID_KEY_DELETE_FWD,
        SEYRUSEFER_HID_KEY_END,
        SEYRUSEFER_HID_KEY_PAGE_DOWN,
        SEYRUSEFER_HID_KEY_RIGHT_ARROW,
        SEYRUSEFER_HID_KEY_LEFT_ARROW,
        SEYRUSEFER_HID_KEY_DOWN_ARROW,
        SEYRUSEFER_HID_KEY_UP_ARROW,
        SEYRUSEFER_HID_KEY_NUM_LOCK,
        SEYRUSEFER_HID_KEY_DIVIDE,
        SEYRUSEFER_HID_KEY_MULTIPLY,
        SEYRUSEFER_HID_KEY_SUBTRACT,
        SEYRUSEFER_HID_KEY_ADD,
        SEYRUSEFER_HID_KEY_ENTER,
        SEYRUSEFER_HID_KEYPAD_1,
        SEYRUSEFER_HID_KEYPAD_2,
        SEYRUSEFER_HID_KEYPAD_3,
        SEYRUSEFER_HID_KEYPAD_4,
        SEYRUSEFER_HID_KEYPAD_5,
        SEYRUSEFER_HID_KEYPAD_6,
        SEYRUSEFER_HID_KEYPAD_7,
        SEYRUSEFER_HID_KEYPAD_8,
        SEYRUSEFER_HID_KEYPAD_9,
        SEYRUSEFER_HID_KEYPAD_0,
        SEYRUSEFER_HID_KEYPAD_DOT,
        SEYRUSEFER_HID_KEY_MUTE,
        SEYRUSEFER_HID_KEY_VOLUME_UP,
        SEYRUSEFER_HID_KEY_VOLUME_DOWN,
        SEYRUSEFER_HID_KEY_LEFT_CTRL,
        SEYRUSEFER_HID_KEY_LEFT_SHIFT,
        SEYRUSEFER_HID_KEY_LEFT_ALT,
        SEYRUSEFER_HID_KEY_LEFT_GUI,
        SEYRUSEFER_HID_KEY_RIGHT_CTRL,
        SEYRUSEFER_HID_KEY_RIGHT_SHIFT,
        SEYRUSEFER_HID_KEY_RIGHT_ALT,
        SEYRUSEFER_HID_KEY_RIGHT_GUI,

        SEYRUSEFER_HID_MOUSE_LEFT,
        SEYRUSEFER_HID_MOUSE_MIDDLE,
        SEYRUSEFER_HID_MOUSE_RIGHT,

        SEYRUSEFER_HID_CONSUMER_POWER,
        SEYRUSEFER_HID_CONSUMER_RESET,
        SEYRUSEFER_HID_CONSUMER_SLEEP,

        SEYRUSEFER_HID_CONSUMER_MENU,
        SEYRUSEFER_HID_CONSUMER_SELECTION,
        SEYRUSEFER_HID_CONSUMER_ASSIGN_SEL,
        SEYRUSEFER_HID_CONSUMER_MODE_STEP,
        SEYRUSEFER_HID_CONSUMER_RECALL_LAST,
        SEYRUSEFER_HID_CONSUMER_QUIT,
        SEYRUSEFER_HID_CONSUMER_HELP,
        SEYRUSEFER_HID_CONSUMER_CHANNEL_UP,
        SEYRUSEFER_HID_CONSUMER_CHANNEL_DOWN,

        SEYRUSEFER_HID_CONSUMER_PLAY,
        SEYRUSEFER_HID_CONSUMER_PAUSE,
        SEYRUSEFER_HID_CONSUMER_RECORD,
        SEYRUSEFER_HID_CONSUMER_FAST_FORWARD,
        SEYRUSEFER_HID_CONSUMER_REWIND,
        SEYRUSEFER_HID_CONSUMER_SCAN_NEXT_TRK,
        SEYRUSEFER_HID_CONSUMER_SCAN_PREV_TRK,
        SEYRUSEFER_HID_CONSUMER_STOP,
        SEYRUSEFER_HID_CONSUMER_EJECT,
        SEYRUSEFER_HID_CONSUMER_RANDOM_PLAY,
        SEYRUSEFER_HID_CONSUMER_SELECT_DISC,
        SEYRUSEFER_HID_CONSUMER_ENTER_DISC,
        SEYRUSEFER_HID_CONSUMER_REPEAT,
        SEYRUSEFER_HID_CONSUMER_STOP_EJECT,
        SEYRUSEFER_HID_CONSUMER_PLAY_PAUSE,
        SEYRUSEFER_HID_CONSUMER_PLAY_SKIP,

        SEYRUSEFER_HID_CONSUMER_VOLUME,
        SEYRUSEFER_HID_CONSUMER_BALANCE,
        SEYRUSEFER_HID_CONSUMER_MUTE,
        SEYRUSEFER_HID_CONSUMER_BASS,
        SEYRUSEFER_HID_CONSUMER_VOLUME_UP,
        SEYRUSEFER_HID_CONSUMER_VOLUME_DOWN,
};

struct seyrusefer_hid_init_options {
        int enabled;
        struct seyrusefer_config *config;
};

int seyrusefer_hid_init_options_default (struct seyrusefer_hid_init_options *options);

struct seyrusefer_hid * seyrusefer_hid_create (struct seyrusefer_hid_init_options *options);
void seyrusefer_hid_destroy (struct seyrusefer_hid *hid);

int seyrusefer_hid_process (struct seyrusefer_hid *hid);

int seyrusefer_hid_start (struct seyrusefer_hid *hid);
int seyrusefer_hid_stop (struct seyrusefer_hid *hid);
int seyrusefer_hid_ap_restart (struct seyrusefer_hid *hid);
int seyrusefer_hid_enabled (struct seyrusefer_hid *hid);

int seyrusefer_hid_connected (struct seyrusefer_hid *hid);
int seyrusefer_hid_send_key (struct seyrusefer_hid *hid, int key, int pressed);
