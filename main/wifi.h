
struct seyrusefer_wifi;

struct seyrusefer_wifi_init_options {
        int enabled;

        void (*callback_ap_start) (struct seyrusefer_wifi *wifi, void *context);
        void (*callback_ap_stop) (struct seyrusefer_wifi *wifi, void *context);

        void *callback_context;
};

struct seyrusefer_wifi_ap_config {
        char ssid[32];
        char password[64];
};

int seyrusefer_wifi_init_options_default (struct seyrusefer_wifi_init_options *options);

struct seyrusefer_wifi * seyrusefer_wifi_create (struct seyrusefer_wifi_init_options *options);
void seyrusefer_wifi_destroy (struct seyrusefer_wifi *wifi);

int seyrusefer_wifi_process (struct seyrusefer_wifi *wifi);

int seyrusefer_wifi_start (struct seyrusefer_wifi *wifi);
int seyrusefer_wifi_stop (struct seyrusefer_wifi *wifi);

int seyrusefer_wifi_ap_get_config (struct seyrusefer_wifi *wifi, struct seyrusefer_wifi_ap_config *config);
int seyrusefer_wifi_ap_set_config (struct seyrusefer_wifi *wifi, struct seyrusefer_wifi_ap_config *config);
int seyrusefer_wifi_ap_restart (struct seyrusefer_wifi *wifi);

int seyrusefer_wifi_enabled (struct seyrusefer_wifi *wifi);
