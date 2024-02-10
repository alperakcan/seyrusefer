
struct seyrusefer_settings;
struct seyrusefer;

struct seyrusefer_init_options {
        int foo;
};

int seyrusefer_init_options_default (struct seyrusefer_init_options *options);

struct seyrusefer * seyrusefer_create (struct seyrusefer_init_options *options);
void seyrusefer_destroy (struct seyrusefer *seyrusefer);

int seyrusefer_process (struct seyrusefer *seyrusefer);
int seyrusefer_get_iteration (struct seyrusefer *seyrusefer);
const struct seyrusefer_settings * seyrusefer_get_settings (struct seyrusefer *seyrusefer);
