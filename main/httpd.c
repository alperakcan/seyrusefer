
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "cJSON.h"

#define SEYRUSEFER_DEBUG_TAG "httpd"
#include "debug.h"
#include "platform.h"
#include "config.h"
#include "httpd.h"

struct seyrusefer_httpd {
        int port;
        int enabled;
        struct seyrusefer_config *config;
        httpd_handle_t server;
};

static esp_err_t api_hello (httpd_req_t *req)
{
        char *resp;
        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        resp = NULL;

        asprintf(&resp, "seyrusefer\n");
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
        free(resp);

        return ESP_OK;
}

static esp_err_t api_system_restart (httpd_req_t *req)
{
        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        httpd_resp_send(req, NULL, 0);
        vTaskDelay(pdMS_TO_TICKS(2000));

        seyrusefer_platform_restart();

        return ESP_OK;
}

static esp_err_t api_settings_get (httpd_req_t *req)
{
        struct seyrusefer_httpd *httpd = req->user_ctx;

        (void) httpd;

        httpd_resp_send(req, "{}", 2);

        return ESP_OK;
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
                .uri      = "/api/hello",
                .method   = HTTP_GET,
                .handler  = api_hello,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/api/system/restart",
                .method   = HTTP_GET,
                .handler  = api_system_restart,
                .user_ctx = NULL
        },
        &(httpd_uri_t) {
                .uri      = "/api/settings/get",
                .method   = HTTP_GET,
                .handler  = api_settings_get,
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
