
struct seyrusefer_config;
struct seyrusefer_httpd;

struct seyrusefer_httpd_init_options {
        int port;
        int enabled;
        struct seyrusefer_config *config;
};

int seyrusefer_httpd_init_options_default (struct seyrusefer_httpd_init_options *options);

struct seyrusefer_httpd * seyrusefer_httpd_create (struct seyrusefer_httpd_init_options *options);
void seyrusefer_httpd_destroy (struct seyrusefer_httpd *httpd);

int seyrusefer_httpd_start (struct seyrusefer_httpd *httpd);
int seyrusefer_httpd_stop (struct seyrusefer_httpd *httpd);
