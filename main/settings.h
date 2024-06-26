
#define SEYRUSEFER_SETTINGS_MAGIC               (('s' << 0x18) | ('e' << 0x10) | ('t' << 0x08) | ('3' << 0x00))

enum {
        SEYRUSEFER_SETTINGS_BUTTON_INVALID      = -1,
        SEYRUSEFER_SETTINGS_BUTTON_1            = 0,
        SEYRUSEFER_SETTINGS_BUTTON_2            = 1,
        SEYRUSEFER_SETTINGS_BUTTON_3            = 2,
        SEYRUSEFER_SETTINGS_BUTTON_4            = 3,
        SEYRUSEFER_SETTINGS_BUTTON_5            = 4,
        SEYRUSEFER_SETTINGS_BUTTON_COUNT        = 5
#define SEYRUSEFER_SETTINGS_BUTTON_INVALID      SEYRUSEFER_SETTINGS_BUTTON_INVALID
#define SEYRUSEFER_SETTINGS_BUTTON_1            SEYRUSEFER_SETTINGS_BUTTON_1
#define SEYRUSEFER_SETTINGS_BUTTON_2            SEYRUSEFER_SETTINGS_BUTTON_2
#define SEYRUSEFER_SETTINGS_BUTTON_3            SEYRUSEFER_SETTINGS_BUTTON_3
#define SEYRUSEFER_SETTINGS_BUTTON_4            SEYRUSEFER_SETTINGS_BUTTON_4
#define SEYRUSEFER_SETTINGS_BUTTON_5            SEYRUSEFER_SETTINGS_BUTTON_5
#define SEYRUSEFER_SETTINGS_BUTTON_COUNT        SEYRUSEFER_SETTINGS_BUTTON_COUNT
};

enum {
        SEYRUSEFER_SETTINGS_MODE_INVALID        = -1,
        SEYRUSEFER_SETTINGS_MODE_1              = 0,
        SEYRUSEFER_SETTINGS_MODE_2              = 1,
        SEYRUSEFER_SETTINGS_MODE_3              = 2,
        SEYRUSEFER_SETTINGS_MODE_4              = 3,
        SEYRUSEFER_SETTINGS_MODE_5              = 4,
        SEYRUSEFER_SETTINGS_MODE_COUNT          = 5
#define SEYRUSEFER_SETTINGS_MODE_INVALID        SEYRUSEFER_SETTINGS_MODE_INVALID
#define SEYRUSEFER_SETTINGS_MODE_1              SEYRUSEFER_SETTINGS_MODE_1
#define SEYRUSEFER_SETTINGS_MODE_2              SEYRUSEFER_SETTINGS_MODE_2
#define SEYRUSEFER_SETTINGS_MODE_3              SEYRUSEFER_SETTINGS_MODE_3
#define SEYRUSEFER_SETTINGS_MODE_4              SEYRUSEFER_SETTINGS_MODE_4
#define SEYRUSEFER_SETTINGS_MODE_5              SEYRUSEFER_SETTINGS_MODE_5
#define SEYRUSEFER_SETTINGS_MODE_COUNT          SEYRUSEFER_SETTINGS_MODE_COUNT
};

struct seyrusefer_settings_button {
        int key;
};

struct seyrusefer_settings_mode {
        struct seyrusefer_settings_button buttons[SEYRUSEFER_SETTINGS_BUTTON_COUNT];
};

struct seyrusefer_settings_led {
        int brightness;
};

struct seyrusefer_settings_key {
        int repeat_delay;
        int repeat_interval;
};

struct seyrusefer_settings {
        uint32_t magic;
        int mode;
        struct seyrusefer_settings_led led;
        struct seyrusefer_settings_key key;
        struct seyrusefer_settings_mode modes[SEYRUSEFER_SETTINGS_MODE_COUNT];
};

const char * seyrusefer_settings_mode_to_string (int mode);
int seyrusefer_settings_mode_from_string (const char *mode);

const char * seyrusefer_settings_button_to_string (int button);
int seyrusefer_settings_button_from_string (const char *button);

const char * seyrusefer_settings_key_to_string (int key);
int seyrusefer_settings_key_from_string (const char *key);
