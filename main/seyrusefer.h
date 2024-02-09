
struct seyrusefer_layout;
struct seyrusefer;

struct seyrusefer_init_options {
        int foo;
};

int seyrusefer_init_options_default (struct seyrusefer_init_options *options);

struct seyrusefer * seyrusefer_create (struct seyrusefer_init_options *options);
void seyrusefer_destroy (struct seyrusefer *seyrusefer);

int seyrusefer_process (struct seyrusefer *seyrusefer);
struct seyrusefer_layout * seyrusefer_layout_get (struct seyrusefer *seyrusefer);
