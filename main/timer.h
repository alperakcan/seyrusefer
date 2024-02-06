
struct seyrusefer_timer;
struct seyrusefer_alarm;

enum {
        SEYRUSEFER_TIMER_RESOLUTION_SECONDS,
        SEYRUSEFER_TIMER_RESOLUTION_500_MILLISECONDS,
        SEYRUSEFER_TIMER_RESOLUTION_250_MILLISECONDS,
        SEYRUSEFER_TIMER_RESOLUTION_100_MILLISECONDS,
        SEYRUSEFER_TIMER_RESOLUTION_50_MILLISECONDS,
        SEYRUSEFER_TIMER_RESOLUTION_25_MILLISECONDS,
        SEYRUSEFER_TIMER_RESOLUTION_MILLISECONDS,
        SEYRUSEFER_TIMER_RESOLUTION_MICROSECONDS,
        SEYRUSEFER_TIMER_RESOLUTION_NANOSECONDS,
        SEYRUSEFER_TIMER_RESOLUTION_DEFAULT             = SEYRUSEFER_TIMER_RESOLUTION_SECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_SECONDS             SEYRUSEFER_TIMER_RESOLUTION_SECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_500_MILLISECONDS    SEYRUSEFER_TIMER_RESOLUTION_500_MILLISECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_250_MILLISECONDS    SEYRUSEFER_TIMER_RESOLUTION_250_MILLISECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_100_MILLISECONDS    SEYRUSEFER_TIMER_RESOLUTION_100_MILLISECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_50_MILLISECONDS     SEYRUSEFER_TIMER_RESOLUTION_50_MILLISECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_25_MILLISECONDS     SEYRUSEFER_TIMER_RESOLUTION_25_MILLISECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_MILLISECONDS        SEYRUSEFER_TIMER_RESOLUTION_MILLISECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_MICROSECONDS        SEYRUSEFER_TIMER_RESOLUTION_MICROSECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_NANOSECONDS         SEYRUSEFER_TIMER_RESOLUTION_NANOSECONDS
#define SEYRUSEFER_TIMER_RESOLUTION_DEFAULT             SEYRUSEFER_TIMER_RESOLUTION_DEFAULT
};

struct seyrusefer_timer_init_options {
        int resolution;
        int enabled;
};

struct seyrusefer_alarm_init_options {
        void (*callback_fired) (struct seyrusefer_alarm *alarm, void *context);
        void *callback_context;
        struct seyrusefer_timer *timer;
        int timeout;
        int singleshot;
        int enabled;
};

int seyrusefer_timer_init_options_default (struct seyrusefer_timer_init_options *options);

struct seyrusefer_timer * seyrusefer_timer_create (struct seyrusefer_timer_init_options *options);
void seyrusefer_timer_destroy (struct seyrusefer_timer *timer);

int seyrusefer_timer_start (struct seyrusefer_timer *timer);
int seyrusefer_timer_stop (struct seyrusefer_timer *timer);
int64_t seyrusefer_timer_get_time (struct seyrusefer_timer *timer);

int seyrusefer_alarm_init_options_default (struct seyrusefer_alarm_init_options *options);

struct seyrusefer_alarm * seyrusefer_alarm_create (struct seyrusefer_alarm_init_options *options);
void seyrusefer_alarm_destroy (struct seyrusefer_alarm *alarm);

int seyrusefer_alarm_start (struct seyrusefer_alarm *alarm);
int seyrusefer_alarm_stop (struct seyrusefer_alarm *alarm);
int seyrusefer_alarm_get_timeout (struct seyrusefer_alarm *alarm);
int seyrusefer_alarm_set_timeout (struct seyrusefer_alarm *alarm, int timeout);
int seyrusefer_alarm_set_singleshot (struct seyrusefer_alarm *alarm, int singleshot);
int seyrusefer_alarm_set_enabled (struct seyrusefer_alarm *alarm, int enabled);
