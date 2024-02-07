
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"

#define SEYRUSEFER_DEBUG_TAG "wifi"
#include "debug.h"
#include "wifi.h"

struct seyrusefer_wifi {
        esp_netif_t *netif_ap;
        esp_event_handler_instance_t wifi_event_handler;

        int enabled;

        char ap_ssid[32];
        char ap_password[64];

        void (*callback_ap_start) (struct seyrusefer_wifi *wifi, void *context);
        void (*callback_ap_stop) (struct seyrusefer_wifi *wifi, void *context);

        void *callback_context;
};

static void wifi_event_handler (void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
        struct seyrusefer_wifi *wifi = arg;

        if (event_id == WIFI_EVENT_WIFI_READY) {
                seyrusefer_debugf("WIFI_EVENT_WIFI_READY");
        } else if (event_id == WIFI_EVENT_SCAN_DONE) {
                seyrusefer_debugf("WIFI_EVENT_SCAN_DONE");
        } else if (event_id == WIFI_EVENT_STA_START) {
                seyrusefer_debugf("WIFI_EVENT_STA_START");
        } else if (event_id == WIFI_EVENT_STA_STOP) {
                seyrusefer_debugf("WIFI_EVENT_STA_STOP");
        } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
                seyrusefer_debugf("WIFI_EVENT_STA_CONNECTED");
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
                seyrusefer_debugf("WIFI_EVENT_STA_DISCONNECTED");
        } else if (event_id == WIFI_EVENT_STA_AUTHMODE_CHANGE) {
                seyrusefer_debugf("WIFI_EVENT_STA_AUTHMODE_CHANGE");
        } else if (event_id == WIFI_EVENT_STA_WPS_ER_SUCCESS) {
                seyrusefer_debugf("WIFI_EVENT_STA_WPS_ER_SUCCESS");
        } else if (event_id == WIFI_EVENT_STA_WPS_ER_FAILED) {
                seyrusefer_debugf("WIFI_EVENT_STA_WPS_ER_FAILED");
        } else if (event_id == WIFI_EVENT_STA_WPS_ER_TIMEOUT) {
                seyrusefer_debugf("WIFI_EVENT_STA_WPS_ER_TIMEOUT");
        } else if (event_id == WIFI_EVENT_STA_WPS_ER_PIN) {
                seyrusefer_debugf("WIFI_EVENT_STA_WPS_ER_PIN");
        } else if (event_id == WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP) {
                seyrusefer_debugf("WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP");
        } else if (event_id == WIFI_EVENT_AP_START) {
                seyrusefer_debugf("WIFI_EVENT_AP_START");
                if (wifi->callback_ap_start != NULL) {
                        wifi->callback_ap_start(wifi, wifi->callback_context);
                }
        } else if (event_id == WIFI_EVENT_AP_STOP) {
                seyrusefer_debugf("WIFI_EVENT_AP_STOP");
                if (wifi->callback_ap_stop != NULL) {
                        wifi->callback_ap_stop(wifi, wifi->callback_context);
                }
        } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
                seyrusefer_debugf("WIFI_EVENT_AP_STACONNECTED");
        } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
                seyrusefer_debugf("WIFI_EVENT_AP_STADISCONNECTED");
        } else if (event_id == WIFI_EVENT_AP_PROBEREQRECVED) {
                seyrusefer_debugf("WIFI_EVENT_AP_PROBEREQRECVED");
        } else if (event_id == WIFI_EVENT_FTM_REPORT) {
                seyrusefer_debugf("WIFI_EVENT_FTM_REPORT");
        } else if (event_id == WIFI_EVENT_STA_BSS_RSSI_LOW) {
                seyrusefer_debugf("WIFI_EVENT_STA_BSS_RSSI_LOW");
        } else if (event_id == WIFI_EVENT_ACTION_TX_STATUS) {
                seyrusefer_debugf("WIFI_EVENT_ACTION_TX_STATUS");
        } else if (event_id == WIFI_EVENT_ROC_DONE) {
                seyrusefer_debugf("WIFI_EVENT_ROC_DONE");
        } else if (event_id == WIFI_EVENT_STA_BEACON_TIMEOUT) {
                seyrusefer_debugf("WIFI_EVENT_STA_BEACON_TIMEOUT");
        } else if (event_id == WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START) {
                seyrusefer_debugf("WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START");
        } else {
                seyrusefer_errorf("unknown wifi event: %ld", event_id);
        }
}

int seyrusefer_wifi_init_options_default (struct seyrusefer_wifi_init_options *options)
{
        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        memset(options, 0, sizeof(struct seyrusefer_wifi_init_options));
        return 0;
bail:   return -1;
}

struct seyrusefer_wifi * seyrusefer_wifi_create (struct seyrusefer_wifi_init_options *options)
{
        int rc;
        struct seyrusefer_wifi *wifi;

        wifi = NULL;

        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }

        wifi = malloc(sizeof(struct seyrusefer_wifi));
        if (wifi == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(wifi, 0, sizeof(struct seyrusefer_wifi));

        wifi->callback_ap_start         = options->callback_ap_start;
        wifi->callback_ap_stop          = options->callback_ap_stop;
        wifi->callback_context          = options->callback_context;

        wifi->netif_ap = esp_netif_create_default_wifi_ap();
        if (wifi->netif_ap == NULL) {
                seyrusefer_errorf("wifi netif_ap is invalid");
                goto bail;
        }

        wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
        rc = esp_wifi_init(&wifi_init_config);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_wifi_init failed, rc: %d, 0x%08x, %s", rc, rc, esp_err_to_name(rc));
                goto bail;
        }
        rc = esp_wifi_set_storage(WIFI_STORAGE_RAM);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_wifi_set_storage failed, rc: %d, 0x%08x, %s", rc, rc, esp_err_to_name(rc));
                goto bail;
        }

        rc = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, wifi, &wifi->wifi_event_handler);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_event_handler_instance_register failed, rc: %d, 0x%08x, %s", rc, rc, esp_err_to_name(rc));
                goto bail;
        }

        rc = esp_wifi_set_mode(WIFI_MODE_NULL);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_wifi_set_mode failed, rc: %d, 0x%08x, %s", rc, rc, esp_err_to_name(rc));
                goto bail;
        }

        if (options->enabled == 1) {
                rc = seyrusefer_wifi_start(wifi);
                if (rc < 0) {
                        seyrusefer_errorf("seyrusefer_wifi_start failed, rc: %d", rc);
                        goto bail;
                }
        }

        return wifi;
bail:   if (wifi != NULL) {
                seyrusefer_wifi_destroy(wifi);
        }
        return NULL;
}

void seyrusefer_wifi_destroy (struct seyrusefer_wifi *wifi)
{
        if (wifi == NULL) {
                return;
        }
        seyrusefer_wifi_stop(wifi);
        if (wifi->wifi_event_handler != NULL) {
                esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi->wifi_event_handler);
        }
        esp_wifi_deinit();
        if (wifi->netif_ap != NULL) {
                esp_netif_destroy_default_wifi(wifi->netif_ap);
        }
        free(wifi);
}

int seyrusefer_wifi_process (struct seyrusefer_wifi *wifi)
{
        if (wifi == NULL) {
                seyrusefer_errorf("wifi is invalid");
                goto bail;
        }
        if (wifi->enabled == 0) {
                seyrusefer_tracef("not enabled");
                goto out;
        }
out:    return 0;
bail:   return -1;
}

int seyrusefer_wifi_start (struct seyrusefer_wifi *wifi)
{
        int rc;
        const char *ap_ssid;
        const char *ap_pass;

        ap_ssid = NULL;
        ap_pass = NULL;

        if (wifi == NULL) {
                seyrusefer_errorf("wifi is invalid");
                goto bail;
        }
        if (wifi->enabled == 1) {
                seyrusefer_debugf("wifi is already started");
                goto out;
        }

        ap_ssid = wifi->ap_ssid;
        if (ap_ssid == NULL || strlen(ap_ssid) == 0) {
                seyrusefer_errorf("ap_ssid is invalid");
                goto bail;
        }
        ap_pass = wifi->ap_password;
        if (ap_pass == NULL || strlen(ap_pass) == 0) {
                seyrusefer_errorf("ap_pass is invalid");
                goto bail;
        }

        rc = esp_wifi_set_mode(WIFI_MODE_AP);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_wifi_set_mode failed, rc: %d, 0x%08x, %s", rc, rc, esp_err_to_name(rc));
                goto bail;
        }

        wifi_config_t wifi_config_ap = {
                .ap = {
                        .ssid           = { 0 },
                        .ssid_len       = 0,
                        .channel        = 1,
                        .password       = { 0 },
                        .max_connection = 16,
                        .authmode       = WIFI_AUTH_WPA_WPA2_PSK,
                        .beacon_interval= 400,
                }
        };
        snprintf((char *) wifi_config_ap.ap.ssid, sizeof(wifi_config_ap.ap.ssid), "%s", ap_ssid);
        snprintf((char *) wifi_config_ap.ap.password, sizeof(wifi_config_ap.ap.password), "%s", ap_pass);
        rc = esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_wifi_set_config ap failed, rc: %d, 0x%08x, %s", rc, rc, esp_err_to_name(rc));
                goto bail;
        }

        rc = esp_wifi_start();
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_wifi_start failed, rc: %d, 0x%08x, %s", rc, rc, esp_err_to_name(rc));
                goto bail;
        }

out:    wifi->enabled = 1;
        return 0;
bail:   return -1;
}

int seyrusefer_wifi_stop (struct seyrusefer_wifi *wifi)
{
        if (wifi == NULL) {
                seyrusefer_errorf("wifi is invalid");
                goto bail;
        }
        if (wifi->enabled == 0) {
                seyrusefer_debugf("wifi is already stopped");
                goto out;
        }

        seyrusefer_errorf("here");
        esp_wifi_stop();
        seyrusefer_errorf("here");
        esp_wifi_restore();
        seyrusefer_errorf("here");

        wifi->enabled = 0;
out:    return 0;
bail:   return -1;
}

int seyrusefer_wifi_ap_get_config (struct seyrusefer_wifi *wifi, struct seyrusefer_wifi_ap_config *config)
{
        int rc;
        wifi_config_t wifi_config;
        if (wifi == NULL) {
                seyrusefer_errorf("wifi is invalid");
                goto bail;
        }
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        rc = esp_wifi_get_config(WIFI_IF_AP, &wifi_config);
        if (rc != ESP_OK) {
                seyrusefer_errorf("esp_wifi_get_config failed, rc: %d, 0x%08x, %s", rc, rc, esp_err_to_name(rc));
                goto bail;
        }
        snprintf(config->ssid, sizeof(config->ssid), "%s", wifi_config.ap.ssid);
        snprintf(config->password, sizeof(config->password), "%s", wifi_config.ap.password);
        return 0;
bail:   return -1;
}

int seyrusefer_wifi_ap_set_config (struct seyrusefer_wifi *wifi, struct seyrusefer_wifi_ap_config *config)
{
        int rc;

        if (wifi == NULL) {
                seyrusefer_errorf("wifi is invalid");
                goto bail;
        }
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }

        snprintf(wifi->ap_ssid, sizeof(wifi->ap_ssid), "%s", config->ssid);
        snprintf(wifi->ap_password, sizeof(wifi->ap_password), "%s", config->password);

        if (wifi->enabled == 1) {
                rc = seyrusefer_wifi_ap_restart(wifi);
                if (rc < 0) {
                        seyrusefer_errorf("seyrusefer_wifi_ap_restart failed");
                        goto bail;
                }
        }

        return 0;
bail:   return -1;
}


int seyrusefer_wifi_ap_restart (struct seyrusefer_wifi *wifi)
{
        int rc;
        if (wifi == NULL) {
                seyrusefer_errorf("wifi is invalid");
                goto bail;
        }
        rc = seyrusefer_wifi_stop(wifi);
        if (rc != 0) {
                seyrusefer_errorf("seyrusefer_wifi_stop failed");
                goto bail;
        }
        rc = seyrusefer_wifi_start(wifi);
        if (rc != 0) {
                seyrusefer_errorf("seyrusefer_wifi_start failed");
                goto bail;
        }
        return 0;
bail:   return -1;
}

int seyrusefer_wifi_enabled (struct seyrusefer_wifi *wifi)
{
        return wifi->enabled;
}
