
struct seyrusefer_config;
struct seyrusefer_hid;

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
