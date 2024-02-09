
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#include "uuid.h"

#define SEYRUSEFER_DEBUG_TAG "hid"
#include "debug.h"
#include "config.h"
#include "hid_hidd_prf_api.h"
#include "hid_dev.h"
#include "hid.h"

struct seyrusefer_hid {
        int enabled;

        uuid_t uuid;

        int sec_conn;
        uint16_t hid_conn_id;

        uint8_t keyboard_buffer[5];
};

#define HIDD_DEVICE_NAME            "Seyrusefer"
static uint8_t hidd_service_uuid128[] = {
        0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x12, 0x18, 0x00, 0x00,
};

static esp_ble_adv_data_t hidd_adv_data = {
        .set_scan_rsp           = false,
        .include_name           = true,
        .include_txpower        = true,
        .min_interval           = 0x0006,       // slave connection min interval, Time = min_interval * 1.25 msec
        .max_interval           = 0x0010,       // slave connection max interval, Time = max_interval * 1.25 msec
        .appearance             = 0x03c1,       // HID Keyboard
        .manufacturer_len       = 0,
        .p_manufacturer_data    = NULL,
        .service_data_len       = 0,
        .p_service_data         = NULL,
        .service_uuid_len       = sizeof(hidd_service_uuid128),
        .p_service_uuid         = hidd_service_uuid128,
        .flag                   = 0x6,
};

static esp_ble_adv_params_t hidd_adv_params = {
        .adv_int_min            = 0x20,
        .adv_int_max            = 0x30,
        .adv_type               = ADV_TYPE_IND,
        .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
#if 0
        .peer_addr               =
        .peer_addr_type         =
#endif
        .channel_map            = ADV_CHNL_ALL,
        .adv_filter_policy      = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static struct seyrusefer_hid *g_seyrusefer_hid = NULL;

static void hidd_event_callback (esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param)
{
        switch(event) {
                case ESP_HIDD_EVENT_REG_FINISH: {
                        if (param->init_finish.state == ESP_HIDD_INIT_OK) {
                                //esp_bd_addr_t rand_addr = {0x04,0x11,0x11,0x11,0x11,0x05};
                                esp_ble_gap_set_device_name(HIDD_DEVICE_NAME);
                                esp_ble_gap_config_adv_data(&hidd_adv_data);
                        }
                        break;
                }
                case ESP_BAT_EVENT_REG: {
                        break;
                }
                case ESP_HIDD_EVENT_DEINIT_FINISH:
                        break;
                case ESP_HIDD_EVENT_BLE_CONNECT: {
                        seyrusefer_infof("ESP_HIDD_EVENT_BLE_CONNECT");
                        g_seyrusefer_hid->hid_conn_id = param->connect.conn_id;
                        break;
                }
                case ESP_HIDD_EVENT_BLE_DISCONNECT: {
                        g_seyrusefer_hid->sec_conn = false;
                        seyrusefer_infof("ESP_HIDD_EVENT_BLE_DISCONNECT");
                        esp_ble_gap_start_advertising(&hidd_adv_params);
                        break;
                }
                case ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT: {
                        seyrusefer_infof("ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT");
                        break;
                }
                case ESP_HIDD_EVENT_BLE_LED_REPORT_WRITE_EVT: {
                        seyrusefer_infof("ESP_HIDD_EVENT_BLE_LED_REPORT_WRITE_EVT");
                        break;
                }
                default:
                        break;
        }
        return;
}

static void gap_event_handler (esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
        switch (event) {
                case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
                        esp_ble_gap_start_advertising(&hidd_adv_params);
                        break;
                case ESP_GAP_BLE_SEC_REQ_EVT:
                        for (int i = 0; i < ESP_BD_ADDR_LEN; i++) {
                                seyrusefer_debugf("%x:",param->ble_security.ble_req.bd_addr[i]);
                        }
                        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
                        break;
                case ESP_GAP_BLE_AUTH_CMPL_EVT:
                        g_seyrusefer_hid->sec_conn = true;
                        esp_bd_addr_t bd_addr;
                        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
                        seyrusefer_infof("remote BD_ADDR: %08x%04x", (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3], (bd_addr[4] << 8) + bd_addr[5]);
                        seyrusefer_infof("address type = %d", param->ble_security.auth_cmpl.addr_type);
                        seyrusefer_infof("pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
                        if (!param->ble_security.auth_cmpl.success) {
                                seyrusefer_errorf("fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
                        }
                        break;
                default:
                        break;
        }
}

int seyrusefer_hid_init_options_default (struct seyrusefer_hid_init_options *options)
{
        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        memset(options, 0, sizeof(struct seyrusefer_hid_init_options));
        return 0;
bail:   return -1;
}

struct seyrusefer_hid * seyrusefer_hid_create (struct seyrusefer_hid_init_options *options)
{
        int rc;
        uuid_t uuid;
        char uuid_str[UUID_STR_LEN];
        char *config_hid_uuid;

        config_hid_uuid = NULL;

        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        if (options->config == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }

again:
        config_hid_uuid = seyrusefer_config_get_string(options->config, "hid_uuid", NULL);
        if (config_hid_uuid != NULL) {
                rc = uuid_parse(config_hid_uuid, uuid);
                if (rc != 0) {
                        seyrusefer_errorf("config_hid_uuid: %s is invalid", config_hid_uuid);
                        free(config_hid_uuid);
                        config_hid_uuid = NULL;
                }
        }
        if (config_hid_uuid == NULL) {
                uuid_generate(uuid);
                uuid_unparse(uuid, uuid_str);
                rc = seyrusefer_config_set_string(options->config, "hid_uuid", uuid_str);
                if (rc != 0) {
                        seyrusefer_errorf("can not set hid_uud config value to: %s", uuid_str);
                        goto bail;
                }
                rc = seyrusefer_config_save(options->config);
                if (rc != 0) {
                        seyrusefer_errorf("can not save config");
                        goto bail;
                }
                goto again;
        }
        seyrusefer_infof("config_hid_uuid: %s", config_hid_uuid);

        g_seyrusefer_hid = malloc(sizeof(struct seyrusefer_hid));
        if (g_seyrusefer_hid == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(g_seyrusefer_hid, 0, sizeof(struct seyrusefer_hid));

        rc = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_bt_controller_mem_release failed, rc: %d", rc);
                goto bail;
        }

        esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
        rc = esp_bt_controller_init(&bt_cfg);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_bt_controller_init failed, rc: %d", rc);
                goto bail;
        }

        rc = esp_bt_controller_enable(ESP_BT_MODE_BLE);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_bt_controller_enable failed, rc: %d", rc);
                goto bail;
        }

        rc = esp_bluedroid_init();
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_bluedroid_init failed, rc: %d", rc);
                goto bail;
        }

        if (options->enabled == 1) {
                rc = seyrusefer_hid_start(g_seyrusefer_hid);
                if (rc < 0) {
                        seyrusefer_errorf("seyrusefer_hid_start failed, rc: %d", rc);
                        goto bail;
                }
        }

        free(config_hid_uuid);
        return g_seyrusefer_hid;
bail:   if (g_seyrusefer_hid != NULL) {
                seyrusefer_hid_destroy(g_seyrusefer_hid);
        }
        if (config_hid_uuid != NULL) {
                free(config_hid_uuid);
        }
        return NULL;
}

void seyrusefer_hid_destroy (struct seyrusefer_hid *hid)
{
        (void) hid;
        if (g_seyrusefer_hid == NULL) {
                return;
        }
        seyrusefer_hid_stop(g_seyrusefer_hid);
        esp_hidd_profile_deinit();
        esp_bluedroid_deinit();
        esp_bt_controller_disable();
        esp_bt_controller_deinit();
        free(g_seyrusefer_hid);
        g_seyrusefer_hid = NULL;
}

int seyrusefer_hid_process (struct seyrusefer_hid *hid)
{
        if (hid == NULL) {
                seyrusefer_errorf("hid is invalid");
                goto bail;
        }
        if (hid->enabled == 0) {
                seyrusefer_tracef("not enabled");
                goto out;
        }
out:    return 0;
bail:   return -1;
}

int seyrusefer_hid_start (struct seyrusefer_hid *hid)
{
        int rc;

        if (hid == NULL) {
                seyrusefer_errorf("hid is invalid");
                goto bail;
        }
        if (hid->enabled == 1) {
                seyrusefer_debugf("hid is already started");
                goto out;
        }

        rc = esp_bluedroid_enable();
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_bluedroid_enable failed, rc: %d", rc);
                goto bail;
        }

        rc = esp_hidd_profile_init();
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_hidd_profile_init failed, rc: %d", rc);
                goto bail;
        }

        esp_ble_gap_register_callback(gap_event_handler);
        esp_hidd_register_callbacks(hidd_event_callback);

        esp_ble_auth_req_t auth_req = ESP_LE_AUTH_BOND;
        esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;
        uint8_t key_size = 16;
        uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
        uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
        esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));

        hid->enabled = 1;
out:    return 0;
bail:   return -1;
}

int seyrusefer_hid_stop (struct seyrusefer_hid *hid)
{
        int rc;

        if (hid == NULL) {
                seyrusefer_errorf("hid is invalid");
                goto bail;
        }
        if (hid->enabled == 0) {
                seyrusefer_debugf("hid is already stopped");
                goto out;
        }

        rc = esp_bluedroid_disable();
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_bluedroid_disable failed, rc: %d", rc);
                goto bail;
        }

        hid->enabled = 0;
out:    return 0;
bail:   return -1;
}

int seyrusefer_hid_ap_restart (struct seyrusefer_hid *hid)
{
        int rc;
        if (hid == NULL) {
                seyrusefer_errorf("hid is invalid");
                goto bail;
        }
        rc = seyrusefer_hid_stop(hid);
        if (rc != 0) {
                seyrusefer_errorf("seyrusefer_hid_stop failed");
                goto bail;
        }
        rc = seyrusefer_hid_start(hid);
        if (rc != 0) {
                seyrusefer_errorf("seyrusefer_hid_start failed");
                goto bail;
        }
        return 0;
bail:   return -1;
}

int seyrusefer_hid_enabled (struct seyrusefer_hid *hid)
{
        return hid->enabled;
}

int seyrusefer_hid_connected (struct seyrusefer_hid *hid)
{
        return hid->sec_conn;
}

static int seyusefer_hid_key_map[] = {
        [SEYRUSEFER_HID_KEY_RESERVED] = HID_KEY_RESERVED,
        [SEYRUSEFER_HID_KEY_A] = HID_KEY_A,
        [SEYRUSEFER_HID_KEY_B] = HID_KEY_B,
        [SEYRUSEFER_HID_KEY_C] = HID_KEY_C,
        [SEYRUSEFER_HID_KEY_D] = HID_KEY_D,
        [SEYRUSEFER_HID_KEY_E] = HID_KEY_E,
        [SEYRUSEFER_HID_KEY_F] = HID_KEY_F,
        [SEYRUSEFER_HID_KEY_G] = HID_KEY_G,
        [SEYRUSEFER_HID_KEY_H] = HID_KEY_H,
        [SEYRUSEFER_HID_KEY_I] = HID_KEY_I,
        [SEYRUSEFER_HID_KEY_J] = HID_KEY_J,
        [SEYRUSEFER_HID_KEY_K] = HID_KEY_K,
        [SEYRUSEFER_HID_KEY_L] = HID_KEY_L,
        [SEYRUSEFER_HID_KEY_M] = HID_KEY_M,
        [SEYRUSEFER_HID_KEY_N] = HID_KEY_N,
        [SEYRUSEFER_HID_KEY_O] = HID_KEY_O,
        [SEYRUSEFER_HID_KEY_P] = HID_KEY_P,
        [SEYRUSEFER_HID_KEY_Q] = HID_KEY_Q,
        [SEYRUSEFER_HID_KEY_R] = HID_KEY_R,
        [SEYRUSEFER_HID_KEY_S] = HID_KEY_S,
        [SEYRUSEFER_HID_KEY_T] = HID_KEY_T,
        [SEYRUSEFER_HID_KEY_U] = HID_KEY_U,
        [SEYRUSEFER_HID_KEY_V] = HID_KEY_V,
        [SEYRUSEFER_HID_KEY_W] = HID_KEY_W,
        [SEYRUSEFER_HID_KEY_X] = HID_KEY_X,
        [SEYRUSEFER_HID_KEY_Y] = HID_KEY_Y,
        [SEYRUSEFER_HID_KEY_Z] = HID_KEY_Z,
        [SEYRUSEFER_HID_KEY_1] = HID_KEY_1,
        [SEYRUSEFER_HID_KEY_2] = HID_KEY_2,
        [SEYRUSEFER_HID_KEY_3] = HID_KEY_3,
        [SEYRUSEFER_HID_KEY_4] = HID_KEY_4,
        [SEYRUSEFER_HID_KEY_5] = HID_KEY_5,
        [SEYRUSEFER_HID_KEY_6] = HID_KEY_6,
        [SEYRUSEFER_HID_KEY_7] = HID_KEY_7,
        [SEYRUSEFER_HID_KEY_8] = HID_KEY_8,
        [SEYRUSEFER_HID_KEY_9] = HID_KEY_9,
        [SEYRUSEFER_HID_KEY_0] = HID_KEY_0,
        [SEYRUSEFER_HID_KEY_RETURN] = HID_KEY_RETURN,
        [SEYRUSEFER_HID_KEY_ESCAPE] = HID_KEY_ESCAPE,
        [SEYRUSEFER_HID_KEY_DELETE] = HID_KEY_DELETE,
        [SEYRUSEFER_HID_KEY_TAB] = HID_KEY_TAB,
        [SEYRUSEFER_HID_KEY_SPACEBAR] = HID_KEY_SPACEBAR,
        [SEYRUSEFER_HID_KEY_MINUS] = HID_KEY_MINUS,
        [SEYRUSEFER_HID_KEY_EQUAL] = HID_KEY_EQUAL,
        [SEYRUSEFER_HID_KEY_LEFT_BRKT] = HID_KEY_LEFT_BRKT,
        [SEYRUSEFER_HID_KEY_RIGHT_BRKT] = HID_KEY_RIGHT_BRKT,
        [SEYRUSEFER_HID_KEY_BACK_SLASH] = HID_KEY_BACK_SLASH,
        [SEYRUSEFER_HID_KEY_SEMI_COLON] = HID_KEY_SEMI_COLON,
        [SEYRUSEFER_HID_KEY_SGL_QUOTE] = HID_KEY_SGL_QUOTE,
        [SEYRUSEFER_HID_KEY_GRV_ACCENT] = HID_KEY_GRV_ACCENT,
        [SEYRUSEFER_HID_KEY_COMMA] = HID_KEY_COMMA,
        [SEYRUSEFER_HID_KEY_DOT] = HID_KEY_DOT,
        [SEYRUSEFER_HID_KEY_FWD_SLASH] = HID_KEY_FWD_SLASH,
        [SEYRUSEFER_HID_KEY_CAPS_LOCK] = HID_KEY_CAPS_LOCK,
        [SEYRUSEFER_HID_KEY_F1] = HID_KEY_F1,
        [SEYRUSEFER_HID_KEY_F2] = HID_KEY_F2,
        [SEYRUSEFER_HID_KEY_F3] = HID_KEY_F3,
        [SEYRUSEFER_HID_KEY_F4] = HID_KEY_F4,
        [SEYRUSEFER_HID_KEY_F5] = HID_KEY_F5,
        [SEYRUSEFER_HID_KEY_F6] = HID_KEY_F6,
        [SEYRUSEFER_HID_KEY_F7] = HID_KEY_F7,
        [SEYRUSEFER_HID_KEY_F8] = HID_KEY_F8,
        [SEYRUSEFER_HID_KEY_F9] = HID_KEY_F9,
        [SEYRUSEFER_HID_KEY_F10] = HID_KEY_F10,
        [SEYRUSEFER_HID_KEY_F11] = HID_KEY_F11,
        [SEYRUSEFER_HID_KEY_F12] = HID_KEY_F12,
        [SEYRUSEFER_HID_KEY_PRNT_SCREEN] = HID_KEY_PRNT_SCREEN,
        [SEYRUSEFER_HID_KEY_SCROLL_LOCK] = HID_KEY_SCROLL_LOCK,
        [SEYRUSEFER_HID_KEY_PAUSE] = HID_KEY_PAUSE,
        [SEYRUSEFER_HID_KEY_INSERT] = HID_KEY_INSERT,
        [SEYRUSEFER_HID_KEY_HOME] = HID_KEY_HOME,
        [SEYRUSEFER_HID_KEY_PAGE_UP] = HID_KEY_PAGE_UP,
        [SEYRUSEFER_HID_KEY_DELETE_FWD] = HID_KEY_DELETE_FWD,
        [SEYRUSEFER_HID_KEY_END] = HID_KEY_END,
        [SEYRUSEFER_HID_KEY_PAGE_DOWN] = HID_KEY_PAGE_DOWN,
        [SEYRUSEFER_HID_KEY_RIGHT_ARROW] = HID_KEY_RIGHT_ARROW,
        [SEYRUSEFER_HID_KEY_LEFT_ARROW] = HID_KEY_LEFT_ARROW,
        [SEYRUSEFER_HID_KEY_DOWN_ARROW] = HID_KEY_DOWN_ARROW,
        [SEYRUSEFER_HID_KEY_UP_ARROW] = HID_KEY_UP_ARROW,
        [SEYRUSEFER_HID_KEY_NUM_LOCK] = HID_KEY_NUM_LOCK,
        [SEYRUSEFER_HID_KEY_DIVIDE] = HID_KEY_DIVIDE,
        [SEYRUSEFER_HID_KEY_MULTIPLY] = HID_KEY_MULTIPLY,
        [SEYRUSEFER_HID_KEY_SUBTRACT] = HID_KEY_SUBTRACT,
        [SEYRUSEFER_HID_KEY_ADD] = HID_KEY_ADD,
        [SEYRUSEFER_HID_KEY_ENTER] = HID_KEY_ENTER,
        [SEYRUSEFER_HID_KEYPAD_1] = HID_KEYPAD_1,
        [SEYRUSEFER_HID_KEYPAD_2] = HID_KEYPAD_2,
        [SEYRUSEFER_HID_KEYPAD_3] = HID_KEYPAD_3,
        [SEYRUSEFER_HID_KEYPAD_4] = HID_KEYPAD_4,
        [SEYRUSEFER_HID_KEYPAD_5] = HID_KEYPAD_5,
        [SEYRUSEFER_HID_KEYPAD_6] = HID_KEYPAD_6,
        [SEYRUSEFER_HID_KEYPAD_7] = HID_KEYPAD_7,
        [SEYRUSEFER_HID_KEYPAD_8] = HID_KEYPAD_8,
        [SEYRUSEFER_HID_KEYPAD_9] = HID_KEYPAD_9,
        [SEYRUSEFER_HID_KEYPAD_0] = HID_KEYPAD_0,
        [SEYRUSEFER_HID_KEYPAD_DOT] = HID_KEYPAD_DOT,
        [SEYRUSEFER_HID_KEY_MUTE] = HID_KEY_MUTE,
        [SEYRUSEFER_HID_KEY_VOLUME_UP] = HID_KEY_VOLUME_UP,
        [SEYRUSEFER_HID_KEY_VOLUME_DOWN] = HID_KEY_VOLUME_DOWN,
        [SEYRUSEFER_HID_KEY_LEFT_CTRL] = HID_KEY_LEFT_CTRL,
        [SEYRUSEFER_HID_KEY_LEFT_SHIFT] = HID_KEY_LEFT_SHIFT,
        [SEYRUSEFER_HID_KEY_LEFT_ALT] = HID_KEY_LEFT_ALT,
        [SEYRUSEFER_HID_KEY_LEFT_GUI] = HID_KEY_LEFT_GUI,
        [SEYRUSEFER_HID_KEY_RIGHT_CTRL] = HID_KEY_RIGHT_CTRL,
        [SEYRUSEFER_HID_KEY_RIGHT_SHIFT] = HID_KEY_RIGHT_SHIFT,
        [SEYRUSEFER_HID_KEY_RIGHT_ALT] = HID_KEY_RIGHT_ALT,
        [SEYRUSEFER_HID_KEY_RIGHT_GUI] = HID_KEY_RIGHT_GUI,
        [SEYRUSEFER_HID_MOUSE_LEFT] = HID_MOUSE_LEFT,
        [SEYRUSEFER_HID_MOUSE_MIDDLE] = HID_MOUSE_MIDDLE,
        [SEYRUSEFER_HID_MOUSE_RIGHT] = HID_MOUSE_RIGHT,
        [SEYRUSEFER_HID_CONSUMER_POWER] = HID_CONSUMER_POWER,
        [SEYRUSEFER_HID_CONSUMER_RESET] = HID_CONSUMER_RESET,
        [SEYRUSEFER_HID_CONSUMER_SLEEP] = HID_CONSUMER_SLEEP,
        [SEYRUSEFER_HID_CONSUMER_MENU] = HID_CONSUMER_MENU,
        [SEYRUSEFER_HID_CONSUMER_SELECTION] = HID_CONSUMER_SELECTION,
        [SEYRUSEFER_HID_CONSUMER_ASSIGN_SEL] = HID_CONSUMER_ASSIGN_SEL,
        [SEYRUSEFER_HID_CONSUMER_MODE_STEP] = HID_CONSUMER_MODE_STEP,
        [SEYRUSEFER_HID_CONSUMER_RECALL_LAST] = HID_CONSUMER_RECALL_LAST,
        [SEYRUSEFER_HID_CONSUMER_QUIT] = HID_CONSUMER_QUIT,
        [SEYRUSEFER_HID_CONSUMER_HELP] = HID_CONSUMER_HELP,
        [SEYRUSEFER_HID_CONSUMER_CHANNEL_UP] = HID_CONSUMER_CHANNEL_UP,
        [SEYRUSEFER_HID_CONSUMER_CHANNEL_DOWN] = HID_CONSUMER_CHANNEL_DOWN,
        [SEYRUSEFER_HID_CONSUMER_PLAY] = HID_CONSUMER_PLAY,
        [SEYRUSEFER_HID_CONSUMER_PAUSE] = HID_CONSUMER_PAUSE,
        [SEYRUSEFER_HID_CONSUMER_RECORD] = HID_CONSUMER_RECORD,
        [SEYRUSEFER_HID_CONSUMER_FAST_FORWARD] = HID_CONSUMER_FAST_FORWARD,
        [SEYRUSEFER_HID_CONSUMER_REWIND] = HID_CONSUMER_REWIND,
        [SEYRUSEFER_HID_CONSUMER_SCAN_NEXT_TRK] = HID_CONSUMER_SCAN_NEXT_TRK,
        [SEYRUSEFER_HID_CONSUMER_SCAN_PREV_TRK] = HID_CONSUMER_SCAN_PREV_TRK,
        [SEYRUSEFER_HID_CONSUMER_STOP] = HID_CONSUMER_STOP,
        [SEYRUSEFER_HID_CONSUMER_EJECT] = HID_CONSUMER_EJECT,
        [SEYRUSEFER_HID_CONSUMER_RANDOM_PLAY] = HID_CONSUMER_RANDOM_PLAY,
        [SEYRUSEFER_HID_CONSUMER_SELECT_DISC] = HID_CONSUMER_SELECT_DISC,
        [SEYRUSEFER_HID_CONSUMER_ENTER_DISC] = HID_CONSUMER_ENTER_DISC,
        [SEYRUSEFER_HID_CONSUMER_REPEAT] = HID_CONSUMER_REPEAT,
        [SEYRUSEFER_HID_CONSUMER_STOP_EJECT] = HID_CONSUMER_STOP_EJECT,
        [SEYRUSEFER_HID_CONSUMER_PLAY_PAUSE] = HID_CONSUMER_PLAY_PAUSE,
        [SEYRUSEFER_HID_CONSUMER_PLAY_SKIP] = HID_CONSUMER_PLAY_SKIP,
        [SEYRUSEFER_HID_CONSUMER_VOLUME] = HID_CONSUMER_VOLUME,
        [SEYRUSEFER_HID_CONSUMER_BALANCE] = HID_CONSUMER_BALANCE,
        [SEYRUSEFER_HID_CONSUMER_MUTE] = HID_CONSUMER_MUTE,
        [SEYRUSEFER_HID_CONSUMER_BASS] = HID_CONSUMER_BASS,
        [SEYRUSEFER_HID_CONSUMER_VOLUME_UP] = HID_CONSUMER_VOLUME_UP,
        [SEYRUSEFER_HID_CONSUMER_VOLUME_DOWN] = HID_CONSUMER_VOLUME_DOWN,
};

int seyrusefer_hid_send_key (struct seyrusefer_hid *hid, int key, int pressed)
{
        if ((key < 0) ||
            (key >= sizeof(seyusefer_hid_key_map) / sizeof(seyusefer_hid_key_map[0]))) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        if (key >= SEYRUSEFER_HID_KEY_A && key <= SEYRUSEFER_HID_KEY_RIGHT_GUI) {
                int send = 1;
                if (key >= SEYRUSEFER_HID_KEY_LEFT_CTRL && key <= SEYRUSEFER_HID_KEY_RIGHT_GUI) {
                        seyrusefer_errorf("key is invalid");
                        goto bail;
                } else {
                        int i;
                        int found = 0;
                        if (pressed) {
                                for (i = 0; i < sizeof(hid->keyboard_buffer); ++i) {
                                        if (hid->keyboard_buffer[i] == 0) {
                                                hid->keyboard_buffer[i] = seyusefer_hid_key_map[key];
                                                found = 1;
                                                break;
                                        }
                                }
                        } else {
                                for (i = 0; i < sizeof(hid->keyboard_buffer); ++i) {
                                        if (!found) {
                                                if (hid->keyboard_buffer[i] == seyusefer_hid_key_map[key]) {
                                                        hid->keyboard_buffer[i] = 0;
                                                        found = 1;
                                                }
                                        } else {
                                                if (hid->keyboard_buffer[i] == 0) {
                                                        break;
                                                }
                                                hid->keyboard_buffer[i-1] = hid->keyboard_buffer[i];
                                                hid->keyboard_buffer[i] = 0;
                                        }
                                }
                        }
                        if (!found) {
                                send = 0;
                        }
                }
                if (send == 1) {
                        esp_hidd_send_keyboard_value(hid->hid_conn_id, 0, hid->keyboard_buffer, sizeof(hid->keyboard_buffer));
                }
        } else if (key >= SEYRUSEFER_HID_CONSUMER_POWER && key <= SEYRUSEFER_HID_CONSUMER_VOLUME_DOWN) {
                esp_hidd_send_consumer_value(hid->hid_conn_id, seyusefer_hid_key_map[key], pressed);
        } else {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        return 0;
bail:   return -1;
}
