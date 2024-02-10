
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"

#define SEYRUSEFER_DEBUG_TAG "seyrusefer"
#include "debug.h"
#include "platform.h"
#include "config.h"
#include "timer.h"
#include "hid.h"
#include "wifi.h"
#include "httpd.h"
#include "settings.h"
#include "seyrusefer.h"

#define SEYRUSEFER_VERSION                              "seyrusefer-1.0.0.bin"

enum {
        SEYRUSEFER_STATE_NONE,
        SEYRUSEFER_STATE_INIT,
        SEYRUSEFER_STATE_RUNNING,
        SEYRUSEFER_STATE_MODE_SELECT,
        SEYRUSEFER_STATE_WIFI_SETUP,
        SEYRUSEFER_STATE_ERROR
#define SEYRUSEFER_STATE_NONE                           SEYRUSEFER_STATE_NONE
#define SEYRUSEFER_STATE_INIT                           SEYRUSEFER_STATE_INIT
#define SEYRUSEFER_STATE_RUNNING                        SEYRUSEFER_STATE_RUNNING
#define SEYRUSEFER_STATE_MODE_SELECT                    SEYRUSEFER_STATE_MODE_SELECT
#define SEYRUSEFER_STATE_WIFI_SETUP                     SEYRUSEFER_STATE_WIFI_SETUP
#define SEYRUSEFER_STATE_ERROR                          SEYRUSEFER_STATE_ERROR
};

struct seyrusefer {
        int iteration;

        int state;
        int pstate;
        int restart;

        int buttons;
        int pbuttons;

        int connected;
        int pconnected;

        int connect_wait_led_brightness;
        int connect_wait_led_brightness_low;
        int connect_wait_led_brightness_high;
        int connect_wait_led_brightness_op;
        int connect_wait_led_brightness_dur;
        int64_t connect_wait_led_brightness_tsms;

        int64_t mode_select_buttons_dur;
        int64_t mode_select_buttons_tsms;

        int mode_select_led_brightness;
        int mode_select_led_brightness_low;
        int mode_select_led_brightness_high;
        int mode_select_led_brightness_dur;
        int64_t mode_select_led_brightness_tsms;

        int64_t wifi_setup_buttons_dur;
        int64_t wifi_setup_buttons_tsms;

        int wifi_setup_led_brightness;
        int wifi_setup_led_brightness_low;
        int wifi_setup_led_brightness_high;
        int wifi_setup_led_brightness_dur;
        int64_t wifi_setup_led_brightness_tsms;

        int stats_dur;
        int64_t stats_tsms;

        struct seyrusefer_config *config;
        struct seyrusefer_timer *timer;
        struct seyrusefer_hid *hid;
        struct seyrusefer_wifi *wifi;
        struct seyrusefer_httpd *httpd;

        struct seyrusefer_alarm *process_alarm;

        struct seyrusefer_settings settings;
};

static const char * seyrusefer_state_string (int state);
static int seyrusefer_set_state_init_actual (struct seyrusefer *seyrusefer);
static int seyrusefer_set_state_running_actual (struct seyrusefer *seyrusefer);
static int seyrusefer_set_state_error_actual (struct seyrusefer *seyrusefer);
static int seyrusefer_set_state (struct seyrusefer *seyrusefer, int state);

static void process_alarm_callback_fired (struct seyrusefer_alarm *alarm, void *context);

static void wifi_callback_ap_start (struct seyrusefer_wifi *wifi, void *context);
static void wifi_callback_ap_stop (struct seyrusefer_wifi *wifi, void *context);

static const char * seyrusefer_state_string (int state)
{
        if (state == SEYRUSEFER_STATE_NONE)             return "none";
        if (state == SEYRUSEFER_STATE_INIT)             return "init";
        if (state == SEYRUSEFER_STATE_RUNNING)          return "running";
        if (state == SEYRUSEFER_STATE_MODE_SELECT)      return "mode-select";
        if (state == SEYRUSEFER_STATE_WIFI_SETUP)       return "wifi-setup";
        if (state == SEYRUSEFER_STATE_ERROR)            return "error";
        return "unknown";
}

static int seyrusefer_set_state_init_actual (struct seyrusefer *seyrusefer)
{
        int rc;
        seyrusefer_platform_set_led(0);
        rc = seyrusefer_timer_start(seyrusefer->timer);
        if (rc < 0) {
                seyrusefer_errorf("can not start timer");
                goto bail;
        }
        rc = seyrusefer_alarm_set_enabled(seyrusefer->process_alarm, 1);
        if (rc < 0) {
                seyrusefer_errorf("can not enable process_alarm");
                goto bail;
        }
        rc = seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_RUNNING);
        if (rc < 0) {
                seyrusefer_errorf("can not set state to rogue");
                goto bail;
        }
        return 0;
bail:   return -1;
}

static int seyrusefer_set_state_running_actual (struct seyrusefer *seyrusefer)
{
        int rc;

#if 1
        if (seyrusefer->pstate == SEYRUSEFER_STATE_WIFI_SETUP) {
                seyrusefer_platform_restart();
                return 0;
        }
#endif

        seyrusefer->connected  = 0;
        seyrusefer->pconnected = -1;

        seyrusefer->mode_select_buttons_tsms = 0;
        seyrusefer->wifi_setup_buttons_tsms  = 0;

        seyrusefer_platform_set_led(0);

        rc = seyrusefer_hid_start(seyrusefer->hid);
        if (rc < 0) {
                seyrusefer_errorf("can not start hid device");
                goto bail;
        }
        rc = seyrusefer_wifi_stop(seyrusefer->wifi);
        if (rc < 0) {
                seyrusefer_errorf("can not stop wifi");
                goto bail;
        }
        rc = seyrusefer_httpd_stop(seyrusefer->httpd);
        if (rc < 0) {
                seyrusefer_errorf("can not stop httpd");
                goto bail;
        }
        return 0;
bail:   return -1;
}

static int seyrusefer_set_state_mode_select_actual (struct seyrusefer *seyrusefer)
{
        int rc;

        seyrusefer->connect_wait_led_brightness_tsms    = 0;

        seyrusefer->mode_select_buttons_tsms            = 0;
        seyrusefer->mode_select_led_brightness_tsms     = 0;

        seyrusefer->wifi_setup_buttons_tsms             = 0;
        seyrusefer->wifi_setup_led_brightness_tsms      = 0;

        seyrusefer_platform_set_led(0);

        rc = seyrusefer_hid_start(seyrusefer->hid);
        if (rc < 0) {
                seyrusefer_errorf("can not start hid device");
                goto bail;
        }
        rc = seyrusefer_wifi_stop(seyrusefer->wifi);
        if (rc < 0) {
                seyrusefer_errorf("can not stop wifi");
                goto bail;
        }
        rc = seyrusefer_httpd_stop(seyrusefer->httpd);
        if (rc < 0) {
                seyrusefer_errorf("can not stop httpd");
                goto bail;
        }
        return 0;
bail:   return -1;
}

static int seyrusefer_set_state_wifi_setup_actual (struct seyrusefer *seyrusefer)
{
        int rc;
        seyrusefer_platform_set_led(0);
        rc = seyrusefer_hid_start(seyrusefer->hid);
        if (rc < 0) {
                seyrusefer_errorf("can not start hid device");
                goto bail;
        }
        rc = seyrusefer_wifi_start(seyrusefer->wifi);
        if (rc < 0) {
                seyrusefer_errorf("can not start wifi");
                goto bail;
        }
        rc = seyrusefer_httpd_start(seyrusefer->httpd);
        if (rc < 0) {
                seyrusefer_errorf("can not start httpd");
                goto bail;
        }
        return 0;
bail:   return -1;
}

static int seyrusefer_set_state_error_actual (struct seyrusefer *seyrusefer)
{
        seyrusefer_errorf("entered in error state, restarting...");
        seyrusefer->restart = 1;
        return 0;
}

static int seyrusefer_set_state (struct seyrusefer *seyrusefer, int state)
{
        int rc;

        if (seyrusefer == NULL) {
                seyrusefer_errorf("seyrusefer is invalid");
                goto bail;
        }

        if (seyrusefer->state == state) {
                seyrusefer_debugf("state is already set");
                goto out;
        }
        seyrusefer->pstate = seyrusefer->state;
        seyrusefer->state  = state;

        seyrusefer_infof("seyrusefer state: %d, %s, pstate: %d, %s", seyrusefer->state, seyrusefer_state_string(seyrusefer->state), seyrusefer->pstate, seyrusefer_state_string(seyrusefer->pstate));

        if (seyrusefer->state == SEYRUSEFER_STATE_INIT) {
                rc = seyrusefer_set_state_init_actual(seyrusefer);
                if (rc != 0) {
                        seyrusefer_errorf("seyrusefer_set_state_init_actual failed");
                        goto bail;
                }
        } else if (seyrusefer->state == SEYRUSEFER_STATE_RUNNING) {
                rc = seyrusefer_set_state_running_actual(seyrusefer);
                if (rc != 0) {
                        seyrusefer_errorf("seyrusefer_set_state_running_actual failed");
                        goto bail;
                }
        } else if (seyrusefer->state == SEYRUSEFER_STATE_MODE_SELECT) {
                rc = seyrusefer_set_state_mode_select_actual(seyrusefer);
                if (rc != 0) {
                        seyrusefer_errorf("seyrusefer_set_state_mode_select_actual failed");
                        goto bail;
                }
        } else if (seyrusefer->state == SEYRUSEFER_STATE_WIFI_SETUP) {
                rc = seyrusefer_set_state_wifi_setup_actual(seyrusefer);
                if (rc != 0) {
                        seyrusefer_errorf("seyrusefer_set_state_wifi_setup_actual failed");
                        goto bail;
                }
        } else if (seyrusefer->state == SEYRUSEFER_STATE_ERROR) {
                rc = seyrusefer_set_state_error_actual(seyrusefer);
                if (rc != 0) {
                        seyrusefer_errorf("seyrusefer_set_state_error_actual failed");
                        goto bail;
                }
        } else {
                seyrusefer_errorf("state is invalid");
                goto bail;
        }

out:    return 0;
bail:   return -1;
}

static void process_alarm_callback_fired (struct seyrusefer_alarm *alarm, void *context)
{
        int rc;
        struct seyrusefer *seyrusefer = context;

        (void) alarm;

        rc = seyrusefer_process(seyrusefer);
        if (rc < 0) {
                seyrusefer_errorf("seyrusefer_process failed, rc: %d", rc);
                goto bail;
        }

        return;
bail:   seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_ERROR);
        return;
}

static void wifi_callback_ap_start (struct seyrusefer_wifi *wifi, void *context)
{
        struct seyrusefer_seyrusefer *seyrusefer = context;
        (void) wifi;
        (void) seyrusefer;
        seyrusefer_infof("wifi ap start");
}

static void wifi_callback_ap_stop (struct seyrusefer_wifi *wifi, void *context)
{
        struct seyrusefer_seyrusefer *seyrusefer = context;
        (void) wifi;
        (void) seyrusefer;
        seyrusefer_infof("wifi ap stop");
}

int seyrusefer_init_options_default (struct seyrusefer_init_options *options)
{
        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        memset(options, 0, sizeof(struct seyrusefer_init_options));
        return 0;
bail:   return -1;
}

struct seyrusefer * seyrusefer_create (struct seyrusefer_init_options *options)
{
        int i;
        int j;

        int rc;
        struct seyrusefer *seyrusefer;
        struct seyrusefer_config_init_options seyrusefer_config_init_options;
        struct seyrusefer_timer_init_options seyrusefer_timer_init_options;
        struct seyrusefer_hid_init_options seyrusefer_hid_init_options;
        struct seyrusefer_wifi_init_options seyrusefer_wifi_init_options;
        struct seyrusefer_httpd_init_options seyrusefer_httpd_init_options;
        struct seyrusefer_alarm_init_options seyrusefer_process_alarm_init_options;

        seyrusefer = NULL;

        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }

        seyrusefer = malloc(sizeof(struct seyrusefer));
        if (seyrusefer == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(seyrusefer, 0, sizeof(struct seyrusefer));

        seyrusefer->state       = SEYRUSEFER_STATE_NONE;
        seyrusefer->pstate      = SEYRUSEFER_STATE_NONE;
        seyrusefer->restart     = 0;

        seyrusefer->buttons     = 0;
        seyrusefer->pbuttons    = 0;

        seyrusefer->connected   = 0;
        seyrusefer->pconnected  = -1;

        seyrusefer->connect_wait_led_brightness_low     = 2;
        seyrusefer->connect_wait_led_brightness_high    = 50;
        seyrusefer->connect_wait_led_brightness_op      = 1;
        seyrusefer->connect_wait_led_brightness_dur     = 20;
        seyrusefer->connect_wait_led_brightness_tsms    = 0;
        seyrusefer->connect_wait_led_brightness         = seyrusefer->connect_wait_led_brightness_low;

        seyrusefer->mode_select_buttons_dur             = 2500;
        seyrusefer->mode_select_buttons_tsms            = 0;

        seyrusefer->mode_select_led_brightness_low      = 2;
        seyrusefer->mode_select_led_brightness_high     = 50;
        seyrusefer->mode_select_led_brightness_dur      = 250;
        seyrusefer->mode_select_led_brightness_tsms     = 0;
        seyrusefer->mode_select_led_brightness          = seyrusefer->mode_select_led_brightness_low;

        seyrusefer->wifi_setup_buttons_dur              = 2500;
        seyrusefer->wifi_setup_buttons_tsms             = 0;

        seyrusefer->wifi_setup_led_brightness_low       = 2;
        seyrusefer->wifi_setup_led_brightness_high      = 50;
        seyrusefer->wifi_setup_led_brightness_dur       = 250;
        seyrusefer->wifi_setup_led_brightness_tsms      = 0;
        seyrusefer->wifi_setup_led_brightness           = seyrusefer->wifi_setup_led_brightness_low;

        seyrusefer->stats_dur                           = 10000;
        seyrusefer->stats_tsms                          = 0;

        seyrusefer->settings.mode = SEYRUSEFER_SETTINGS_MODE_1;
        for (i = 0; i < SEYRUSEFER_SETTINGS_MODE_COUNT; i++) {
                for (j = 0; j < SEYRUSEFER_SETTINGS_BUTTON_COUNT; j++) {
                        seyrusefer->settings.modes[i].buttons[j].key = 0;
                }
        }

        seyrusefer->settings.magic = SEYRUSEFER_SETTINGS_MAGIC;
        seyrusefer->settings.mode  = SEYRUSEFER_SETTINGS_MODE_1;

        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_1].buttons[SEYRUSEFER_SETTINGS_BUTTON_1].key = SEYRUSEFER_HID_KEY_C;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_1].buttons[SEYRUSEFER_SETTINGS_BUTTON_2].key = SEYRUSEFER_HID_KEY_R;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_1].buttons[SEYRUSEFER_SETTINGS_BUTTON_3].key = SEYRUSEFER_HID_KEY_D;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_1].buttons[SEYRUSEFER_SETTINGS_BUTTON_4].key = SEYRUSEFER_HID_CONSUMER_VOLUME_UP;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_1].buttons[SEYRUSEFER_SETTINGS_BUTTON_5].key = SEYRUSEFER_HID_CONSUMER_VOLUME_DOWN;

        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_2].buttons[SEYRUSEFER_SETTINGS_BUTTON_1].key = SEYRUSEFER_HID_CONSUMER_VOLUME_UP;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_2].buttons[SEYRUSEFER_SETTINGS_BUTTON_2].key = SEYRUSEFER_HID_CONSUMER_VOLUME_DOWN;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_2].buttons[SEYRUSEFER_SETTINGS_BUTTON_3].key = SEYRUSEFER_HID_CONSUMER_SCAN_NEXT_TRK;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_2].buttons[SEYRUSEFER_SETTINGS_BUTTON_4].key = SEYRUSEFER_HID_CONSUMER_SCAN_PREV_TRK;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_2].buttons[SEYRUSEFER_SETTINGS_BUTTON_5].key = SEYRUSEFER_HID_CONSUMER_SCAN_NEXT_TRK;

        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_3].buttons[SEYRUSEFER_SETTINGS_BUTTON_1].key = SEYRUSEFER_HID_CONSUMER_PLAY_PAUSE;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_3].buttons[SEYRUSEFER_SETTINGS_BUTTON_2].key = SEYRUSEFER_HID_CONSUMER_SCAN_NEXT_TRK;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_3].buttons[SEYRUSEFER_SETTINGS_BUTTON_3].key = SEYRUSEFER_HID_CONSUMER_SCAN_PREV_TRK;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_3].buttons[SEYRUSEFER_SETTINGS_BUTTON_4].key = SEYRUSEFER_HID_CONSUMER_VOLUME_UP;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_3].buttons[SEYRUSEFER_SETTINGS_BUTTON_5].key = SEYRUSEFER_HID_CONSUMER_VOLUME_DOWN;

        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_4].buttons[SEYRUSEFER_SETTINGS_BUTTON_1].key = SEYRUSEFER_HID_KEY_RESERVED;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_4].buttons[SEYRUSEFER_SETTINGS_BUTTON_2].key = SEYRUSEFER_HID_KEY_RESERVED;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_4].buttons[SEYRUSEFER_SETTINGS_BUTTON_3].key = SEYRUSEFER_HID_KEY_RESERVED;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_4].buttons[SEYRUSEFER_SETTINGS_BUTTON_4].key = SEYRUSEFER_HID_KEY_RESERVED;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_4].buttons[SEYRUSEFER_SETTINGS_BUTTON_5].key = SEYRUSEFER_HID_KEY_RESERVED;

        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_5].buttons[SEYRUSEFER_SETTINGS_BUTTON_1].key = SEYRUSEFER_HID_KEY_RESERVED;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_5].buttons[SEYRUSEFER_SETTINGS_BUTTON_2].key = SEYRUSEFER_HID_KEY_RESERVED;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_5].buttons[SEYRUSEFER_SETTINGS_BUTTON_3].key = SEYRUSEFER_HID_KEY_RESERVED;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_5].buttons[SEYRUSEFER_SETTINGS_BUTTON_4].key = SEYRUSEFER_HID_KEY_RESERVED;
        seyrusefer->settings.modes[SEYRUSEFER_SETTINGS_MODE_5].buttons[SEYRUSEFER_SETTINGS_BUTTON_5].key = SEYRUSEFER_HID_KEY_RESERVED;

        seyrusefer_infof("creating config");
        seyrusefer_infof("  path     : %s", "config");
        rc = seyrusefer_config_init_options_default(&seyrusefer_config_init_options);
        if (rc < 0) {
                seyrusefer_errorf("can not init config init options");
                goto bail;
        }
        seyrusefer_config_init_options.path = "config";
        seyrusefer->config = seyrusefer_config_create(&seyrusefer_config_init_options);
        if (seyrusefer->config == NULL) {
                seyrusefer_errorf("can not create config");
                goto bail;
        }

        {
                int length;
                struct seyrusefer_settings *settings;
                rc = seyrusefer_config_get_blob(seyrusefer->config, "settings", (void **) &settings, &length);
                if (rc == 0) {
                        seyrusefer_infof("writing settings to config");
                        rc = seyrusefer_config_set_blob(seyrusefer->config, "settings", &seyrusefer->settings, sizeof(struct seyrusefer_settings));
                        if (rc != 0) {
                                seyrusefer_errorf("can not set config");
                                goto bail;
                        }
                } else if (rc == 1) {
                        seyrusefer_infof("using settings from config");
                        if (length == sizeof(struct seyrusefer_settings) &&
                            settings->magic == seyrusefer->settings.magic) {
                                memcpy(&seyrusefer->settings, settings, sizeof(struct seyrusefer_settings));
                        }
                        free(settings);
                } else {
                        seyrusefer_errorf("can not get config");
                        goto bail;
                }
        }

        seyrusefer_infof("creating timer");
        seyrusefer_infof("  enabled   : %d", 0);
        seyrusefer_infof("  resolution: %s", "default");
        rc = seyrusefer_timer_init_options_default(&seyrusefer_timer_init_options);
        if (rc < 0) {
                seyrusefer_errorf("can not init timer init options");
                goto bail;
        }
        seyrusefer_timer_init_options.resolution = SEYRUSEFER_TIMER_RESOLUTION_MILLISECONDS;
        seyrusefer_timer_init_options.enabled    = 0;
        seyrusefer->timer = seyrusefer_timer_create(&seyrusefer_timer_init_options);
        if (seyrusefer->timer == NULL) {
                seyrusefer_errorf("can not create timer");
                goto bail;
        }

        seyrusefer_infof("creating hid");
        seyrusefer_infof("  enabled : %d", 0);
        rc = seyrusefer_hid_init_options_default(&seyrusefer_hid_init_options);
        if (rc < 0) {
                seyrusefer_errorf("can not init hid init options");
                goto bail;
        }
        seyrusefer_hid_init_options.enabled     = 0;
        seyrusefer_hid_init_options.config      = seyrusefer->config;
        seyrusefer->hid = seyrusefer_hid_create(&seyrusefer_hid_init_options);
        if (seyrusefer->hid == NULL) {
                seyrusefer_errorf("can not create hid");
                goto bail;
        }

        seyrusefer_infof("creating wifi");
        seyrusefer_infof("  enabled : %d", 0);
        rc = seyrusefer_wifi_init_options_default(&seyrusefer_wifi_init_options);
        if (rc < 0) {
                seyrusefer_errorf("can not init wifi init options");
                goto bail;
        }
        seyrusefer_wifi_init_options.enabled                     = 0;
        seyrusefer_wifi_init_options.config                      = seyrusefer->config;
        seyrusefer_wifi_init_options.callback_ap_start           = wifi_callback_ap_start;
        seyrusefer_wifi_init_options.callback_ap_stop            = wifi_callback_ap_stop;
        seyrusefer_wifi_init_options.callback_context            = seyrusefer;
        seyrusefer->wifi = seyrusefer_wifi_create(&seyrusefer_wifi_init_options);
        if (seyrusefer->wifi == NULL) {
                seyrusefer_errorf("can not create wifi");
                goto bail;
        }

        seyrusefer_infof("creating httpd");
        seyrusefer_infof("  port   : %d", 80);
        seyrusefer_infof("  enabled: %d", 0);
        rc = seyrusefer_httpd_init_options_default(&seyrusefer_httpd_init_options);
        if (rc < 0) {
                seyrusefer_errorf("can not init httpd init options");
                goto bail;
        }
        seyrusefer_httpd_init_options.port    = 80;
        seyrusefer_httpd_init_options.enabled = 0;
        seyrusefer_httpd_init_options.config  = seyrusefer->config;
        seyrusefer->httpd = seyrusefer_httpd_create(&seyrusefer_httpd_init_options);
        if (seyrusefer->httpd == NULL) {
                seyrusefer_errorf("can not create httpd");
                goto bail;
        }

        seyrusefer_infof("creating process alarm");
        seyrusefer_infof("  enabled: %d", 0);
        rc = seyrusefer_alarm_init_options_default(&seyrusefer_process_alarm_init_options);
        if (rc < 0) {
                seyrusefer_errorf("can not init alarm init options");
                goto bail;
        }
        seyrusefer_process_alarm_init_options.callback_fired     = process_alarm_callback_fired;
        seyrusefer_process_alarm_init_options.callback_context   = seyrusefer;
        seyrusefer_process_alarm_init_options.timer              = seyrusefer->timer;
        seyrusefer_process_alarm_init_options.timeout            = 10000;
        seyrusefer_process_alarm_init_options.singleshot         = 0;
        seyrusefer_process_alarm_init_options.enabled            = 0;
        seyrusefer->process_alarm = seyrusefer_alarm_create(&seyrusefer_process_alarm_init_options);
        if (seyrusefer->process_alarm == NULL) {
                seyrusefer_errorf("can not create process alarm");
                goto bail;
        }

        {
	        const esp_partition_t *running_partition;

		running_partition = esp_ota_get_running_partition();

                seyrusefer_infof("running_partition: %p", running_partition);
                seyrusefer_infof("  label  : %s", running_partition->label);
                seyrusefer_infof("  type   : %d", running_partition->type);
                seyrusefer_infof("  subtype: %d", running_partition->subtype);
        }

        rc = seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_INIT);
        if (rc != 0) {
                seyrusefer_errorf("can not set seyrusefer state to init");
                goto bail;
        }

        return seyrusefer;
bail:   if (seyrusefer != NULL) {
                seyrusefer_destroy(seyrusefer);
        }
        return NULL;
}

void seyrusefer_destroy (struct seyrusefer *seyrusefer)
{
        if (seyrusefer == NULL) {
                return;
        }
        if (seyrusefer->process_alarm != NULL) {
                seyrusefer_alarm_destroy(seyrusefer->process_alarm);
        }
        if (seyrusefer->httpd != NULL) {
                seyrusefer_httpd_destroy(seyrusefer->httpd);
        }
        if (seyrusefer->wifi != NULL) {
                seyrusefer_wifi_destroy(seyrusefer->wifi);
        }
        if (seyrusefer->hid != NULL) {
                seyrusefer_hid_destroy(seyrusefer->hid);
        }
        if (seyrusefer->timer != NULL) {
                seyrusefer_timer_destroy(seyrusefer->timer);
        }
        if (seyrusefer->config != NULL) {
                seyrusefer_config_destroy(seyrusefer->config);
        }
        free(seyrusefer);
}

int seyrusefer_process (struct seyrusefer *seyrusefer)
{
        int rc;
        int64_t now;

        int buttons_pressed;
        int buttons_released;
        int buttons_active;

        seyrusefer->iteration += 1;

        now = seyrusefer_timer_get_time(seyrusefer->timer);

        buttons_pressed  = 0;
        buttons_released = 0;
        buttons_active   = 0;

        if (seyrusefer->restart) {
                seyrusefer_errorf("restart requested, in 1 second");
                vTaskDelay(pdMS_TO_TICKS(1000));
                seyrusefer_platform_restart();
                return 0;
        }

        if (now < seyrusefer->stats_tsms ||
            now - seyrusefer->stats_tsms >= seyrusefer->stats_dur * 1000) {
                seyrusefer_debugf("stats:");
                seyrusefer_debugf("  freeheap: %ld", esp_get_free_heap_size());
                seyrusefer->stats_tsms = now;
        }

        rc = seyrusefer_hid_process(seyrusefer->hid);
        if (rc < 0) {
                seyrusefer_errorf("seyrusefer_hid_process failed, rc: %d", rc);
                goto bail;
        }
        rc = seyrusefer_wifi_process(seyrusefer->wifi);
        if (rc < 0) {
                seyrusefer_errorf("seyrusefer_wifi_process failed, rc: %d", rc);
                goto bail;
        }

        seyrusefer->buttons   = seyrusefer_platform_get_buttons();
        seyrusefer->connected = seyrusefer_hid_connected(seyrusefer->hid);

        if (seyrusefer->buttons != seyrusefer->pbuttons) {
                buttons_pressed  = seyrusefer->buttons & ~seyrusefer->pbuttons;
                buttons_released = seyrusefer->pbuttons & ~seyrusefer->buttons;
                buttons_active   = buttons_pressed | buttons_released;
                seyrusefer_debugf("buttons: 0x%08x, pressed: 0x%08x, released: 0x%08x (b: 0x%08x, p: 0x%08x)", buttons_active, buttons_pressed, buttons_released, seyrusefer->buttons, seyrusefer->pbuttons);
        }

        if (seyrusefer->connected != seyrusefer->pconnected) {
                seyrusefer_debugf("connected: %d", seyrusefer->connected);
        }

        if (seyrusefer->state == SEYRUSEFER_STATE_RUNNING) {
                if (seyrusefer->connected != seyrusefer->pconnected) {
                        if (seyrusefer->connected == 0) {
                                seyrusefer->connect_wait_led_brightness_tsms = now;
                                seyrusefer->connect_wait_led_brightness      = seyrusefer->connect_wait_led_brightness_low;
                                seyrusefer_platform_set_led(seyrusefer->connect_wait_led_brightness);
                        } else if (seyrusefer->connected == 1) {
                                seyrusefer_platform_set_led(0);
                        }
                }

                if (seyrusefer->connected == 0) {
                        if (now < seyrusefer->connect_wait_led_brightness_tsms ||
                            now - seyrusefer->connect_wait_led_brightness_tsms >= seyrusefer->connect_wait_led_brightness_dur * 1000) {
                                if (seyrusefer->connect_wait_led_brightness > seyrusefer->connect_wait_led_brightness_high) {
                                        seyrusefer->connect_wait_led_brightness    = seyrusefer->connect_wait_led_brightness_high;
                                        seyrusefer->connect_wait_led_brightness_op = -1;
                                } else if (seyrusefer->connect_wait_led_brightness < seyrusefer->connect_wait_led_brightness_low) {
                                        seyrusefer->connect_wait_led_brightness    = seyrusefer->connect_wait_led_brightness_low;
                                        seyrusefer->connect_wait_led_brightness_op = +1;
                                } else {
                                        seyrusefer->connect_wait_led_brightness += seyrusefer->connect_wait_led_brightness_op;
                                }
                                seyrusefer_platform_set_led(seyrusefer->connect_wait_led_brightness);
                                seyrusefer->connect_wait_led_brightness_tsms = now;
                        }
                } else if (seyrusefer->connected == 1) {
                        if (seyrusefer->buttons != seyrusefer->pbuttons) {
                                seyrusefer_platform_set_led(seyrusefer->buttons ? 50 : 0);

#if 0
                                if (buttons_active & SEYRUSEFER_PLATFORM_BUTTON_1) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_1].key, buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_1);
                                }
                                if (buttons_active & SEYRUSEFER_PLATFORM_BUTTON_2) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_2].key, buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_2);
                                }
                                if (buttons_active & SEYRUSEFER_PLATFORM_BUTTON_3) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_3].key, buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_3);
                                }
                                if (buttons_active & SEYRUSEFER_PLATFORM_BUTTON_4) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_4].key, buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_4);
                                }
                                if (buttons_active & SEYRUSEFER_PLATFORM_BUTTON_5) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_5].key, buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_5);
                                }
#else
                                if (buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_1) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_1].key, 1);
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_1].key, 0);
                                }
                                if (buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_2) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_2].key, 1);
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_2].key, 0);
                                }
                                if (buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_3) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_3].key, 1);
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_3].key, 0);
                                }
                                if (buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_4) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_4].key, 1);
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_4].key, 0);
                                }
                                if (buttons_pressed & SEYRUSEFER_PLATFORM_BUTTON_5) {
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_5].key, 1);
                                        seyrusefer_hid_send_key(seyrusefer->hid, seyrusefer->settings.modes[seyrusefer->settings.mode].buttons[SEYRUSEFER_SETTINGS_BUTTON_5].key, 0);
                                }
#endif
                        }
                } else {
                        seyrusefer_errorf("seyrusefer_hid_connected failed, rc: %d", rc);
                        goto bail;
                }

                if (seyrusefer->buttons == (SEYRUSEFER_PLATFORM_BUTTON_1 | SEYRUSEFER_PLATFORM_BUTTON_2)) {
                        if (seyrusefer->mode_select_buttons_tsms == 0) {
                                seyrusefer->mode_select_buttons_tsms = now;
                        }
                        if (now < seyrusefer->mode_select_buttons_tsms ||
                            now - seyrusefer->mode_select_buttons_tsms >= seyrusefer->mode_select_buttons_dur * 1000) {
                                seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_MODE_SELECT);
                        }
                } else {
                        seyrusefer->mode_select_buttons_tsms = 0;
                }

                if (seyrusefer->buttons == (SEYRUSEFER_PLATFORM_BUTTON_1 | SEYRUSEFER_PLATFORM_BUTTON_3)) {
                        if (seyrusefer->wifi_setup_buttons_tsms == 0) {
                                seyrusefer->wifi_setup_buttons_tsms = now;
                        }
                        if (now < seyrusefer->wifi_setup_buttons_tsms ||
                            now - seyrusefer->wifi_setup_buttons_tsms >= seyrusefer->wifi_setup_buttons_dur * 1000) {
                                seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_WIFI_SETUP);
                        }
                } else {
                        seyrusefer->wifi_setup_buttons_tsms = 0;
                }
        } else if (seyrusefer->state == SEYRUSEFER_STATE_MODE_SELECT) {
                if (now < seyrusefer->mode_select_led_brightness_tsms ||
                    now - seyrusefer->mode_select_led_brightness_tsms >= seyrusefer->mode_select_led_brightness_dur * 1000) {
                        if (seyrusefer->mode_select_led_brightness == seyrusefer->mode_select_led_brightness_high) {
                                seyrusefer->mode_select_led_brightness = seyrusefer->mode_select_led_brightness_low;
                        } else if (seyrusefer->mode_select_led_brightness == seyrusefer->mode_select_led_brightness_low) {
                                seyrusefer->mode_select_led_brightness = seyrusefer->mode_select_led_brightness_high;
                        } else {
                                seyrusefer->mode_select_led_brightness = seyrusefer->mode_select_led_brightness_low;
                        }
                        seyrusefer_platform_set_led(seyrusefer->mode_select_led_brightness);
                        seyrusefer->mode_select_led_brightness_tsms = now;
                }
                if (seyrusefer->buttons != seyrusefer->pbuttons &&
                    seyrusefer->pbuttons == 0) {
                        if (seyrusefer->buttons == SEYRUSEFER_PLATFORM_BUTTON_1) {
                                seyrusefer_debugf("mode-1");
                                seyrusefer->settings.mode = SEYRUSEFER_SETTINGS_MODE_1;
                                seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_RUNNING);
                        } else if (seyrusefer->buttons == SEYRUSEFER_PLATFORM_BUTTON_2) {
                                seyrusefer_debugf("mode-2");
                                seyrusefer->settings.mode = SEYRUSEFER_SETTINGS_MODE_2;
                                seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_RUNNING);
                        } else if (seyrusefer->buttons == SEYRUSEFER_PLATFORM_BUTTON_3) {
                                seyrusefer_debugf("mode-3");
                                seyrusefer->settings.mode = SEYRUSEFER_SETTINGS_MODE_3;
                                seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_RUNNING);
                        } else if (seyrusefer->buttons == SEYRUSEFER_PLATFORM_BUTTON_4) {
                                seyrusefer_debugf("mode-4");
                                seyrusefer->settings.mode = SEYRUSEFER_SETTINGS_MODE_4;
                                seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_RUNNING);
                        } else if (seyrusefer->buttons == SEYRUSEFER_PLATFORM_BUTTON_5) {
                                seyrusefer_debugf("mode-5");
                                seyrusefer->settings.mode = SEYRUSEFER_SETTINGS_MODE_5;
                                seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_RUNNING);
                        }
                        seyrusefer_infof("writing settings to config");
                        rc = seyrusefer_config_set_blob(seyrusefer->config, "settings", &seyrusefer->settings, sizeof(struct seyrusefer_settings));
                        if (rc != 0) {
                                seyrusefer_errorf("can not set config");
                                goto bail;
                        }
                }
        } else if (seyrusefer->state == SEYRUSEFER_STATE_WIFI_SETUP) {
                if (now < seyrusefer->wifi_setup_led_brightness_tsms ||
                    now - seyrusefer->wifi_setup_led_brightness_tsms >= seyrusefer->wifi_setup_led_brightness_dur * 1000) {
                        if (seyrusefer->wifi_setup_led_brightness == seyrusefer->wifi_setup_led_brightness_high) {
                                seyrusefer->wifi_setup_led_brightness = seyrusefer->wifi_setup_led_brightness_low;
                        } else if (seyrusefer->wifi_setup_led_brightness == seyrusefer->wifi_setup_led_brightness_low) {
                                seyrusefer->wifi_setup_led_brightness = seyrusefer->wifi_setup_led_brightness_high;
                        } else {
                                seyrusefer->wifi_setup_led_brightness = seyrusefer->wifi_setup_led_brightness_low;
                        }
                        seyrusefer_platform_set_led(seyrusefer->wifi_setup_led_brightness);
                        seyrusefer->wifi_setup_led_brightness_tsms = now;
                }
                if (seyrusefer->buttons != seyrusefer->pbuttons &&
                    seyrusefer->pbuttons == 0) {
                        seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_RUNNING);
                }
        } else {
                seyrusefer_errorf("seyrusefer state: %d is invalid", seyrusefer->state);
                goto bail;
        }

        seyrusefer->pbuttons   = seyrusefer->buttons;
        seyrusefer->pconnected = seyrusefer->connected;
        return 0;
bail:   return -1;
}

int seyrusefer_get_iteration (struct seyrusefer *seyrusefer)
{
        if (seyrusefer == NULL) {
                seyrusefer_errorf("seyrusefer is invalid");
                goto bail;
        }
        return seyrusefer->iteration;
bail:   return -1;
}

const struct seyrusefer_settings * seyrusefer_get_settings (struct seyrusefer *seyrusefer)
{
        if (seyrusefer == NULL) {
                seyrusefer_errorf("seyrusefer is invalid");
                goto bail;
        }
        return &seyrusefer->settings;
bail:   return NULL;
}
