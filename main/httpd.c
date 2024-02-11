
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "cJSON.h"

#define SEYRUSEFER_DEBUG_TAG "httpd"
#include "debug.h"
#include "platform.h"
#include "config.h"
#include "hid.h"
#include "settings.h"
#include "httpd.h"

struct seyrusefer_httpd {
        int port;
        int enabled;
        struct seyrusefer_config *config;
        httpd_handle_t server;
};

static esp_err_t system_version (httpd_req_t *req)
{
        char *resp;
        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        resp = NULL;

        asprintf(&resp, "seyrusefer-1.0.0.bin");
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
        free(resp);

        return ESP_OK;
}

static esp_err_t api_system_restart (httpd_req_t *req)
{
        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        httpd_resp_send(req, "{}", 2);
        vTaskDelay(pdMS_TO_TICKS(2000));

        seyrusefer_platform_restart();

        return ESP_OK;
}

static esp_err_t api_system_restore (httpd_req_t *req)
{
        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        seyrusefer_config_clear(httpd->config);

        httpd_resp_send(req, "{}", 2);

        return ESP_OK;
}

static esp_err_t api_settings_get (httpd_req_t *req)
{
        int rc;
        int length;
        struct seyrusefer_settings *settings;

        struct seyrusefer_httpd *httpd = req->user_ctx;

        rc = seyrusefer_config_get_blob(httpd->config, "settings", (void **) &settings, &length);
        if (rc == 0) {
                httpd_resp_send(req, "{}", 2);
        } else if (rc == 1) {
                char *resp;
                rc = asprintf(&resp,
                        "{" \
                        "  \"mode\":\"%s\"," \
                        "  \"led\": {" \
                        "    \"brightness\": \"%d\"" \
                        "  }," \
                        "  \"modes\":[{" \
                        "    \"buttons\":[" \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }" \
                        "    ]" \
                        "   },{" \
                        "    \"buttons\":[" \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }" \
                        "    ]" \
                        "   },{" \
                        "    \"buttons\":[" \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }" \
                        "    ]" \
                        "   },{" \
                        "    \"buttons\":[" \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }" \
                        "    ]" \
                        "   },{" \
                        "    \"buttons\":[" \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }," \
                        "      { \"key\":\"%s\" }" \
                        "    ]" \
                        "   }]" \
                        "}",
                        seyrusefer_settings_mode_to_string(settings->mode),
                        settings->led.brightness,
                        seyrusefer_settings_key_to_string(settings->modes[0].buttons[0].key),
                        seyrusefer_settings_key_to_string(settings->modes[0].buttons[1].key),
                        seyrusefer_settings_key_to_string(settings->modes[0].buttons[2].key),
                        seyrusefer_settings_key_to_string(settings->modes[0].buttons[3].key),
                        seyrusefer_settings_key_to_string(settings->modes[0].buttons[4].key),
                        seyrusefer_settings_key_to_string(settings->modes[1].buttons[0].key),
                        seyrusefer_settings_key_to_string(settings->modes[1].buttons[1].key),
                        seyrusefer_settings_key_to_string(settings->modes[1].buttons[2].key),
                        seyrusefer_settings_key_to_string(settings->modes[1].buttons[3].key),
                        seyrusefer_settings_key_to_string(settings->modes[1].buttons[4].key),
                        seyrusefer_settings_key_to_string(settings->modes[2].buttons[0].key),
                        seyrusefer_settings_key_to_string(settings->modes[2].buttons[1].key),
                        seyrusefer_settings_key_to_string(settings->modes[2].buttons[2].key),
                        seyrusefer_settings_key_to_string(settings->modes[2].buttons[3].key),
                        seyrusefer_settings_key_to_string(settings->modes[2].buttons[4].key),
                        seyrusefer_settings_key_to_string(settings->modes[3].buttons[0].key),
                        seyrusefer_settings_key_to_string(settings->modes[3].buttons[1].key),
                        seyrusefer_settings_key_to_string(settings->modes[3].buttons[2].key),
                        seyrusefer_settings_key_to_string(settings->modes[3].buttons[3].key),
                        seyrusefer_settings_key_to_string(settings->modes[3].buttons[4].key),
                        seyrusefer_settings_key_to_string(settings->modes[4].buttons[0].key),
                        seyrusefer_settings_key_to_string(settings->modes[4].buttons[1].key),
                        seyrusefer_settings_key_to_string(settings->modes[4].buttons[2].key),
                        seyrusefer_settings_key_to_string(settings->modes[4].buttons[3].key),
                        seyrusefer_settings_key_to_string(settings->modes[4].buttons[4].key)
                );
                if (rc < 0) {
                        seyrusefer_errorf("can not allocate memory");
                        goto bail;
                }
                httpd_resp_send(req, resp, strlen(resp));
                free(resp);
                free(settings);
        } else {
                seyrusefer_errorf("can not get config");
                goto bail;
        }

        return ESP_OK;
bail:   httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal Server Error");
        return ESP_FAIL;
}

static esp_err_t api_settings_set (httpd_req_t *req)
{
        int r;
        char *buffer;
        cJSON *root;
        cJSON *mode;
        cJSON *led;
        cJSON *ledBrightness;
        cJSON *modes;
        cJSON *button;
        cJSON *buttons;
        cJSON *key;

        int m;
        int b;
        int rc;
        struct seyrusefer_settings settings;
        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        root = NULL;
        buffer = NULL;

        buffer = malloc(req->content_len + 1);
        if (buffer == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(buffer, 0, req->content_len + 1);

        r = 0;
        while (r < req->content_len) {
                rc = httpd_req_recv(req, buffer + r, req->content_len - r);
                if (rc <= 0) {
                        seyrusefer_errorf("httpd_req_recv failed, rc: %d, r: %d / %d", rc, r, req->content_len);
                        goto bail;
                }
                r += rc;
        }
        if (r != req->content_len) {
                seyrusefer_errorf("httpd_req_recv failed, r: %d != %d", r, req->content_len);
                goto bail;
        }

        memset(&settings, 0, sizeof(struct seyrusefer_settings));
        settings.magic = SEYRUSEFER_SETTINGS_MAGIC;

        root = cJSON_Parse(buffer);
        if (root == NULL) {
                seyrusefer_errorf("can not parse buffer");
                goto bail;
        }

        mode = cJSON_GetObjectItem(root, "mode");
        if (mode == NULL || !cJSON_IsString(mode)) {
                seyrusefer_errorf("mode is invalid");
                goto bail;
        }
        settings.mode = seyrusefer_settings_mode_from_string(mode->valuestring);
        if (settings.mode == SEYRUSEFER_SETTINGS_MODE_INVALID) {
                seyrusefer_errorf("mode: %s is invalid", mode->valuestring);
                goto bail;
        }

        led = cJSON_GetObjectItem(root, "led");
        if (led == NULL || !cJSON_IsObject(led)) {
                seyrusefer_errorf("led is invalid");
                goto bail;
        }
        ledBrightness = cJSON_GetObjectItem(led, "brightness");
        if (ledBrightness == NULL || !cJSON_IsString(ledBrightness)) {
                seyrusefer_errorf("ledBrightness is invalid");
                goto bail;
        }
        settings.led.brightness = atoi(ledBrightness->valuestring);
        if (settings.led.brightness < 20) {
                settings.led.brightness = 20;
        }

        modes = cJSON_GetObjectItem(root, "modes");
        if (modes == NULL || !cJSON_IsArray(modes)) {
                seyrusefer_errorf("modes is invalid");
                goto bail;
        }

        m = 0;
        cJSON_ArrayForEach(mode, modes) {
                if (m >= SEYRUSEFER_SETTINGS_MODE_COUNT) {
                        seyrusefer_errorf("settings is invalid");
                        goto bail;
                }

                if (mode == NULL || !cJSON_IsObject(mode)) {
                        seyrusefer_errorf("mode is invalid");
                        goto bail;
                }
                buttons = cJSON_GetObjectItem(mode, "buttons");
                if (buttons == NULL || !cJSON_IsArray(buttons)) {
                        seyrusefer_errorf("buttons is invalid");
                        goto bail;
                }

                b = 0;
                cJSON_ArrayForEach(button, buttons) {
                        if (b >= SEYRUSEFER_SETTINGS_BUTTON_COUNT) {
                                seyrusefer_errorf("settings is invalid");
                                goto bail;
                        }

                        if (button == NULL || !cJSON_IsObject(button)) {
                                seyrusefer_errorf("button is invalid");
                                goto bail;
                        }
                        key = cJSON_GetObjectItem(button, "key");
                        if (key == NULL || !cJSON_IsString(key)) {
                                seyrusefer_errorf("key is invalid");
                                goto bail;
                        }

                        settings.modes[m].buttons[b].key = seyrusefer_settings_key_from_string(key->valuestring);
                        if (settings.modes[m].buttons[b].key == SEYRUSEFER_HID_KEY_INVALID) {
                                seyrusefer_errorf("key: %s is invalid", key->valuestring);
                                goto bail;
                        }

                        b += 1;
                }
                if (b != SEYRUSEFER_SETTINGS_BUTTON_COUNT) {
                        seyrusefer_errorf("settings is invalid");
                        goto bail;
                }

                m += 1;
        }
        if (m != SEYRUSEFER_SETTINGS_MODE_COUNT) {
                seyrusefer_errorf("settings is invalid");
                goto bail;
        }

        seyrusefer_infof("writing settings to config");
        rc = seyrusefer_config_set_blob(httpd->config, "settings", &settings, sizeof(struct seyrusefer_settings));
        if (rc != 0) {
                seyrusefer_errorf("can not set config");
                goto bail;
        }

        cJSON_Delete(root);
        free(buffer);

        httpd_resp_send(req, "{}", 2);
        return ESP_OK;
bail:   httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal Server Error");
        if (buffer != NULL) {
                free(buffer);
        }
        if (root != NULL) {
                cJSON_Delete(root);
        }
        return ESP_FAIL;
}

static esp_err_t www_index_html (httpd_req_t *req)
{
        extern const unsigned char www_index_html_start[] asm("_binary_index_html_start");
        extern const unsigned char www_index_html_end[]   asm("_binary_index_html_end");
        const size_t www_index_html_size = (www_index_html_end - www_index_html_start);

        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, (const char *) www_index_html_start, www_index_html_size);

        return ESP_OK;
}

static esp_err_t www_bootstrap_icons_css (httpd_req_t *req)
{
        extern const unsigned char www_bootstrap_icons_css_start[] asm("_binary_bootstrap_icons_css_start");
        extern const unsigned char www_bootstrap_icons_css_end[]   asm("_binary_bootstrap_icons_css_end");
        const size_t www_bootstrap_icons_css_size = (www_bootstrap_icons_css_end - www_bootstrap_icons_css_start);

        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        httpd_resp_set_type(req, "text/css");
        httpd_resp_send(req, (const char *) www_bootstrap_icons_css_start, www_bootstrap_icons_css_size);

        return ESP_OK;
}

static esp_err_t www_bootstrap_min_css (httpd_req_t *req)
{
        extern const unsigned char www_bootstrap_min_css_start[] asm("_binary_bootstrap_min_css_start");
        extern const unsigned char www_bootstrap_min_css_end[]   asm("_binary_bootstrap_min_css_end");
        const size_t www_bootstrap_min_css_size = (www_bootstrap_min_css_end - www_bootstrap_min_css_start);

        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        httpd_resp_set_type(req, "text/css");
        httpd_resp_send(req, (const char *) www_bootstrap_min_css_start, www_bootstrap_min_css_size);

        return ESP_OK;
}

static esp_err_t www_bootstrap_min_js (httpd_req_t *req)
{
        extern const unsigned char www_bootstrap_min_js_start[] asm("_binary_bootstrap_min_js_start");
        extern const unsigned char www_bootstrap_min_js_end[]   asm("_binary_bootstrap_min_js_end");
        const size_t www_bootstrap_min_js_size = (www_bootstrap_min_js_end - www_bootstrap_min_js_start);

        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        httpd_resp_set_type(req, "text/javascript");
        httpd_resp_send(req, (const char *) www_bootstrap_min_js_start, www_bootstrap_min_js_size);

        return ESP_OK;
}

static esp_err_t www_jquery_min_js (httpd_req_t *req)
{
        extern const unsigned char www_jquery_min_js_start[] asm("_binary_jquery_min_js_start");
        extern const unsigned char www_jquery_min_js_end[]   asm("_binary_jquery_min_js_end");
        const size_t www_jquery_min_js_size = (www_jquery_min_js_end - www_jquery_min_js_start);

        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        httpd_resp_set_type(req, "text/javascript");
        httpd_resp_send(req, (const char *) www_jquery_min_js_start, www_jquery_min_js_size);

        return ESP_OK;
}

static esp_err_t www_popper_min_js (httpd_req_t *req)
{
        extern const unsigned char www_popper_min_js_start[] asm("_binary_popper_min_js_start");
        extern const unsigned char www_popper_min_js_end[]   asm("_binary_popper_min_js_end");
        const size_t www_popper_min_js_size = (www_popper_min_js_end - www_popper_min_js_start);

        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        httpd_resp_set_type(req, "text/javascript");
        httpd_resp_send(req, (const char *) www_popper_min_js_start, www_popper_min_js_size);

        return ESP_OK;
}

static httpd_uri_t *apis[] = {
        &(httpd_uri_t) {
                .uri      = "/api/system/version",
                .method   = HTTP_GET,
                .handler  = system_version,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/api/system/restart",
                .method   = HTTP_GET,
                .handler  = api_system_restart,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/api/system/restore",
                .method   = HTTP_GET,
                .handler  = api_system_restore,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/api/settings/get",
                .method   = HTTP_GET,
                .handler  = api_settings_get,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/api/settings/set",
                .method   = HTTP_POST,
                .handler  = api_settings_set,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/",
                .method   = HTTP_GET,
                .handler  = www_index_html,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/index.html",
                .method   = HTTP_GET,
                .handler  = www_index_html,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/bootstrap-icons.css",
                .method   = HTTP_GET,
                .handler  = www_bootstrap_icons_css,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/bootstrap.min.css",
                .method   = HTTP_GET,
                .handler  = www_bootstrap_min_css,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/bootstrap.min.js",
                .method   = HTTP_GET,
                .handler  = www_bootstrap_min_js,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/jquery.min.js",
                .method   = HTTP_GET,
                .handler  = www_jquery_min_js,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/popper.min.js",
                .method   = HTTP_GET,
                .handler  = www_popper_min_js,
                .user_ctx = NULL
        },
};

int seyrusefer_httpd_init_options_default (struct seyrusefer_httpd_init_options *options)
{
        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        memset(options, 0, sizeof(struct seyrusefer_httpd_init_options));
        return 0;
bail:   return -1;
}

struct seyrusefer_httpd * seyrusefer_httpd_create (struct seyrusefer_httpd_init_options *options)
{
        int i;
        int rc;
        struct seyrusefer_httpd *httpd;

        httpd = NULL;

        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        if (options->config == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }

        httpd = malloc(sizeof(struct seyrusefer_httpd));
        if (httpd == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(httpd, 0, sizeof(struct seyrusefer_httpd));

        httpd->port   = options->port;
        httpd->config = options->config;

        for (i = 0; i < sizeof(apis) / sizeof(apis[0]); i++) {
                apis[i]->user_ctx = httpd;
        }

        if (options->enabled == 1) {
                rc = seyrusefer_httpd_start(httpd);
                if (rc < 0) {
                        seyrusefer_errorf("seyrusefer_httpd_start failed, rc: %d", rc);
                        goto bail;
                }
        }

        return httpd;
bail:   if (httpd != NULL) {
                seyrusefer_httpd_destroy(httpd);
        }
        return NULL;
}

void seyrusefer_httpd_destroy (struct seyrusefer_httpd *httpd)
{
        if (httpd == NULL) {
                return;
        }
        seyrusefer_httpd_stop(httpd);
        free(httpd);
}

int seyrusefer_httpd_start (struct seyrusefer_httpd *httpd)
{
        int i;
        int rc;

        if (httpd == NULL) {
                seyrusefer_errorf("httpd is invalid");
                goto bail;
        }
        if (httpd->enabled == 1) {
                seyrusefer_debugf("httpd is already started");
                goto out;
        }

        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.server_port = httpd->port;
        config.max_uri_handlers = sizeof(apis) / sizeof(apis[0]);

        rc = httpd_start(&httpd->server, &config);
        if (rc != ESP_OK) {
                seyrusefer_errorf("httpd_start failed, rc: %d", rc);
                goto bail;
        }

        for (i = 0; i < sizeof(apis) / sizeof(apis[0]); i++) {
                rc = httpd_register_uri_handler(httpd->server, apis[i]);
                if (rc != ESP_OK) {
                        seyrusefer_errorf("httpd_reqister_uri_handler failed, rc: %d", rc);
                        goto bail;
                }
        }

        httpd->enabled = 1;
out:    return 0;
bail:   return -1;
}

int seyrusefer_httpd_stop (struct seyrusefer_httpd *httpd)
{
        if (httpd == NULL) {
                seyrusefer_errorf("httpd is invalid");
                goto bail;
        }
        if (httpd->enabled == 0) {
                seyrusefer_debugf("httpd is already stopped");
                goto out;
        }

        if (httpd->server != NULL) {
                httpd_stop(httpd->server);
                httpd->server = NULL;
        }

        httpd->enabled = 0;
out:    return 0;
bail:   return -1;
}
