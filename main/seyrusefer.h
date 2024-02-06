
struct seyrusefer;

struct seyrusefer_init_options {
        int foo;
};

int seyrusefer_init_options_default (struct seyrusefer_init_options *options);

struct seyrusefer * seyrusefer_create (struct seyrusefer_init_options *options);
void seyrusefer_destroy (struct seyrusefer *seyrusefer);

int seyrusefer_process (struct seyrusefer *seyrusefer);
