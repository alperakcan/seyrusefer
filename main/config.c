
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"

#define SEYRUSEFER_DEBUG_TAG "config"
#include "debug.h"
#include "config.h"

struct seyrusefer_config {
        char *path;
        char *name;
        nvs_handle_t nvs;
};

int seyrusefer_config_init_options_default (struct seyrusefer_config_init_options *options)
{
        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        memset(options, 0, sizeof(struct seyrusefer_config_init_options));
        return 0;
bail:   return -1;
}

struct seyrusefer_config * seyrusefer_config_create (struct seyrusefer_config_init_options *options)
{
        int rc;
        struct seyrusefer_config *config;

        config = NULL;

        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        if (options->path == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }

        config = malloc(sizeof(struct seyrusefer_config));
        if (config == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(config, 0, sizeof(struct seyrusefer_config));

        config->path = strdup(options->path);
        if (config->path == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        config->name = strdup(options->path);
        if (config->name == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }

        rc = nvs_open_from_partition(config->path, config->name, NVS_READWRITE, &config->nvs);
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not open config: %s:%s, rc: %d, 0x%08x, 0x%08x", config->path, config->name, rc, rc, ESP_ERR_NVS_PART_NOT_FOUND);
                goto bail;
        }

        return config;
bail:   if (config != NULL) {
                seyrusefer_config_destroy(config);
        }
        return NULL;
}

void seyrusefer_config_destroy (struct seyrusefer_config *config)
{
        if (config == NULL) {
                return;
        }
        if (config->nvs != 0) {
                nvs_close(config->nvs);
        }
        if (config->name != NULL) {
                free(config->name);
        }
        if (config->path != NULL) {
                free(config->path);
        }
        free(config);
}

int seyrusefer_config_clear (struct seyrusefer_config *config)
{
        int rc;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        rc = nvs_erase_all(config->nvs);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_erase_all failed, rc: %d", rc);
                goto bail;
        }
        return 0;
bail:   return -1;
}

int seyrusefer_config_del (struct seyrusefer_config *config, const char *key)
{
        int rc;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_erase_key(config->nvs, key);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_erase_all failed, rc: %d", rc);
                goto bail;
        }
        return 0;
bail:   return -1;
}

int seyrusefer_config_set (struct seyrusefer_config *config, const char *key, const char *value)
{
        int rc;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        if (value == NULL) {
                seyrusefer_errorf("value is invalid");
                goto bail;
        }
        rc = nvs_set_str(config->nvs, key, value);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_set_str failed, rc: %d", rc);
                goto bail;
        }
        return 0;
bail:   return -1;
}

int seyrusefer_config_set_uint8 (struct seyrusefer_config *config, const char *key, uint8_t value)
{
        int rc;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_set_u8(config->nvs, key, value);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_set_u8 failed, rc: %d", rc);
                goto bail;
        }
        return 0;
bail:   return -1;
}

int seyrusefer_config_set_uint16 (struct seyrusefer_config *config, const char *key, uint16_t value)
{
        int rc;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_set_u16(config->nvs, key, value);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_set_u16 failed, rc: %d", rc);
                goto bail;
        }
        return 0;
bail:   return -1;
}

int seyrusefer_config_set_uint32 (struct seyrusefer_config *config, const char *key, uint32_t value)
{
        int rc;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_set_u32(config->nvs, key, value);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_set_u32 failed, rc: %d", rc);
                goto bail;
        }
        return 0;
bail:   return -1;
}

int seyrusefer_config_set_string (struct seyrusefer_config *config, const char *key, const char *value)
{
        return seyrusefer_config_set(config, key, value);
}

int seyrusefer_config_set_blob (struct seyrusefer_config *config, const char *key, const void *value, int length)
{
        int rc;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_set_blob(config->nvs, key, value, length);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_set_blob failed, rc: %d", rc);
                goto bail;
        }
        return 0;
bail:   return -1;
}

char * seyrusefer_config_get (struct seyrusefer_config *config, const char *key)
{
        int rc;
        char *value;
        size_t length;
        value = NULL;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_get_str(config->nvs, key, value, &length);
        if (rc == ESP_ERR_NVS_NOT_FOUND) {
                goto out;
        }
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_get_str failed, rc: %d", rc);
                goto bail;
        }
        value = malloc(length + 1);
        if (value == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(value, 0, length + 1);
        rc = nvs_get_str(config->nvs, key, value, &length);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_get_str failed, rc: %d", rc);
                goto bail;
        }
out:    return value;
bail:   if (value != NULL) {
                free(value);
        }
        return NULL;
}

uint8_t seyrusefer_config_get_uint8 (struct seyrusefer_config *config, const char *key, uint8_t value)
{
        int rc;
        uint8_t val;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_get_u8(config->nvs, key, &val);
        if (rc == ESP_ERR_NVS_NOT_FOUND) {
                val = value;
                goto out;
        }
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_get_u8 failed, rc: %d", rc);
                goto bail;
        }
out:    return val;
bail:   return value;
}

uint16_t seyrusefer_config_get_uint16 (struct seyrusefer_config *config, const char *key, uint16_t value)
{
        int rc;
        uint16_t val;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_get_u16(config->nvs, key, &val);
        if (rc == ESP_ERR_NVS_NOT_FOUND) {
                val = value;
                goto out;
        }
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_get_u16 failed, rc: %d", rc);
                goto bail;
        }
out:    return val;
bail:   return value;
}

uint32_t seyrusefer_config_get_uint32 (struct seyrusefer_config *config, const char *key, uint32_t value)
{
        int rc;
        uint32_t val;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_get_u32(config->nvs, key, &val);
        if (rc == ESP_ERR_NVS_NOT_FOUND) {
                val = value;
                goto out;
        }
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_get_u32 failed, rc: %d", rc);
                goto bail;
        }
out:    return val;
bail:   return value;
}

char * seyrusefer_config_get_string (struct seyrusefer_config *config, const char *key, const char *value)
{
        char *tmp;
        tmp = seyrusefer_config_get(config, key);
        if (tmp == NULL) {
                return strdup(value);
        }
        return tmp;
}

int seyrusefer_config_get_blob (struct seyrusefer_config *config, const char *key, void **value, int *length)
{
        int rc;
        int rs;
        char *tvalue;
        size_t tlength;
        rs = 0;
        tvalue  = NULL;
        tlength = 0;
        if (value != NULL) {
                *value = NULL;
        }
        if (length != NULL) {
                *length = 0;
        }
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        if (key == NULL) {
                seyrusefer_errorf("key is invalid");
                goto bail;
        }
        rc = nvs_get_blob(config->nvs, key, tvalue, &tlength);
        if (rc == ESP_ERR_NVS_NOT_FOUND) {
                goto out;
        }
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_get_blob failed, rc: %d", rc);
                goto bail;
        }
        tvalue = malloc(tlength + 1);
        if (tvalue == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(tvalue, 0, tlength + 1);
        rc = nvs_get_blob(config->nvs, key, tvalue, &tlength);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_get_blob failed, rc: %d", rc);
                goto bail;
        }
        if (value != NULL) {
                *value = tvalue;
        } else {
                free(tvalue);
        }
        if (length != NULL) {
                *length = tlength;
        }
        rs = 1;
out:    return rs;
bail:   if (tvalue != NULL) {
                free(tvalue);
        }
        return -1;
}

int seyrusefer_config_print (struct seyrusefer_config *config)
{
        int rc;
        nvs_iterator_t it;
        it = NULL;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        rc = nvs_entry_find(config->path, config->name, NVS_TYPE_ANY, &it);
        if (rc == ESP_ERR_NVS_NOT_FOUND) {
                goto out;
        }
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not create find iterator");
                goto bail;
        }
        while (rc == ESP_OK) {
                nvs_entry_info_t info;
                nvs_entry_info(it, &info);

                rc = nvs_entry_next(&it);
                if (rc == ESP_ERR_NVS_NOT_FOUND) {
                        break;
                }
                if (rc != ESP_OK) {
                        seyrusefer_errorf("can not create find iterator");
                        goto bail;
                }
        }
out:    nvs_release_iterator(it);
        return 0;
bail:   if (it != NULL) {
                nvs_release_iterator(it);
        }
        return -1;
}

int seyrusefer_config_save (struct seyrusefer_config *config)
{
        int rc;
        if (config == NULL) {
                seyrusefer_errorf("config is invalid");
                goto bail;
        }
        rc = nvs_commit(config->nvs);
        if (rc != ESP_OK) {
                seyrusefer_errorf("nvs_commit failed, rc: %d", rc);
                goto bail;
        }
        return 0;
bail:   return -1;
}
