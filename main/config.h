
struct seyrusefer_config;

struct seyrusefer_config_init_options {
        const char *path;
};

int seyrusefer_config_init_options_default (struct seyrusefer_config_init_options *options);

struct seyrusefer_config * seyrusefer_config_create (struct seyrusefer_config_init_options *options);
void seyrusefer_config_destroy (struct seyrusefer_config *config);

char * seyrusefer_config_get (struct seyrusefer_config *config, const char *key);
uint8_t seyrusefer_config_get_uint8 (struct seyrusefer_config *config, const char *key, uint8_t value);
uint16_t seyrusefer_config_get_uint16 (struct seyrusefer_config *config, const char *key, uint16_t value);
uint32_t seyrusefer_config_get_uint32 (struct seyrusefer_config *config, const char *key, uint32_t value);
char * seyrusefer_config_get_string (struct seyrusefer_config *config, const char *key, const char *value);
int seyrusefer_config_get_blob (struct seyrusefer_config *config, const char *key, void **value, int *length);

int seyrusefer_config_set (struct seyrusefer_config *config, const char *key, const char *value);
int seyrusefer_config_set_uint8 (struct seyrusefer_config *config, const char *key, uint8_t value);
int seyrusefer_config_set_uint16 (struct seyrusefer_config *config, const char *key, uint16_t value);
int seyrusefer_config_set_uint32 (struct seyrusefer_config *config, const char *key, uint32_t value);
int seyrusefer_config_set_string (struct seyrusefer_config *config, const char *key, const char *value);
int seyrusefer_config_set_blob (struct seyrusefer_config *config, const char *key, const void *value, int length);

int seyrusefer_config_clear (struct seyrusefer_config *config);
int seyrusefer_config_del (struct seyrusefer_config *config, const char *key);

int seyrusefer_config_print (struct seyrusefer_config *config);
int seyrusefer_config_save (struct seyrusefer_config *config);
