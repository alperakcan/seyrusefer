
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include "esp_log.h"

#define SEYRUSEFER_DEBUG_TAG "timer"
#include "debug.h"
#include "queue.h"
#include "timer.h"

ESP_EVENT_DECLARE_BASE(SEYRUSEFER_TIMER_EVENTS);
ESP_EVENT_DEFINE_BASE(SEYRUSEFER_TIMER_EVENTS);

enum {
        SEYRUSEFER_TIMER_EVENT_FIRED
#define SEYRUSEFER_TIMER_EVENT_FIRED     SEYRUSEFER_TIMER_EVENT_FIRED
};

struct seyrusefer_timer_event_fired {
        int64_t time;
};

TAILQ_HEAD(seyrusefer_alarms, seyrusefer_alarm);
struct seyrusefer_alarm {
        TAILQ_ENTRY(seyrusefer_alarm) tailq;
        void (*callback_fired) (struct seyrusefer_alarm *alarm, void *context);
        void *callback_context;
        struct seyrusefer_timer *timer;
        int timeout;
        int enabled;
        int singleshot;
        int64_t firetime;
};

struct seyrusefer_timer {
        esp_timer_handle_t esp_timer;
        esp_event_handler_instance_t timer_event_fired_handler;
        struct seyrusefer_alarms alarms;
        int resolution;
        int enabled;
};

static void timer_event_fired_handler (void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
        struct seyrusefer_alarm *alarm;
        struct seyrusefer_alarm *nalarm;

        struct seyrusefer_timer *timer = handler_args;
        struct seyrusefer_timer_event_fired *seyrusefer_timer_event_fired = event_data;

        seyrusefer_tracef("timer event, time: %lld us", seyrusefer_timer_event_fired->time);

        TAILQ_FOREACH_SAFE(alarm, &timer->alarms, tailq, nalarm) {
                if (alarm->enabled == 0) {
                        continue;
                }
                if (alarm->firetime <= seyrusefer_timer_event_fired->time) {
                        if (alarm->singleshot == 1) {
                                alarm->enabled = 0;
                        }
                        alarm->firetime = alarm->firetime + alarm->timeout;
                        if (alarm->callback_fired != NULL) {
                                taskYIELD();
                                alarm->callback_fired(alarm, alarm->callback_context);
                        }
                }
        }
}

static void esp_timer_callback (void *arg)
{
        struct seyrusefer_timer_event_fired seyrusefer_timer_event_fired;
        seyrusefer_timer_event_fired.time = esp_timer_get_time();
        esp_event_post(SEYRUSEFER_TIMER_EVENTS, SEYRUSEFER_TIMER_EVENT_FIRED, &seyrusefer_timer_event_fired, sizeof(seyrusefer_timer_event_fired), portMAX_DELAY);
}

int seyrusefer_timer_init_options_default (struct seyrusefer_timer_init_options *options)
{
        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        memset(options, 0, sizeof(struct seyrusefer_timer_init_options));
        options->resolution = SEYRUSEFER_TIMER_RESOLUTION_DEFAULT;
        return 0;
bail:   return -1;
}

struct seyrusefer_timer * seyrusefer_timer_create (struct seyrusefer_timer_init_options *options)
{
        int rc;
        struct seyrusefer_timer *timer;

        timer = NULL;

        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }

        timer = malloc(sizeof(struct seyrusefer_timer));
        if (timer == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(timer, 0, sizeof(struct seyrusefer_timer));
        timer->resolution = options->resolution;
        TAILQ_INIT(&timer->alarms);

        rc = esp_event_handler_instance_register(SEYRUSEFER_TIMER_EVENTS, SEYRUSEFER_TIMER_EVENT_FIRED, timer_event_fired_handler, timer, &timer->timer_event_fired_handler);
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not register to timer event, rc: %d", rc);
                goto bail;
        }

        const esp_timer_create_args_t esp_timer_create_args = {
                .callback               = &esp_timer_callback,
                .arg                    = timer,
                .dispatch_method        = ESP_TIMER_TASK,
                .name                   = "seyrusefer-timer",
                .skip_unhandled_events  = 1
        };
        rc = esp_timer_create(&esp_timer_create_args, &timer->esp_timer);
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not create esp_timer");
                goto bail;
        }

        if (options->enabled) {
                rc = seyrusefer_timer_start(timer);
                if (rc < 0) {
                        seyrusefer_errorf("can not start timer");
                        goto bail;
                }
        }

        return timer;
bail:   if (timer != NULL) {
                seyrusefer_timer_destroy(timer);
        }
        return NULL;
}

void seyrusefer_timer_destroy (struct seyrusefer_timer *timer)
{
        struct seyrusefer_alarm *alarm;
        struct seyrusefer_alarm *nalarm;
        if (timer == NULL) {
                return;
        }
        TAILQ_FOREACH_SAFE(alarm, &timer->alarms, tailq, nalarm) {
                TAILQ_REMOVE(&timer->alarms, alarm, tailq);
                alarm->timer = NULL;
        }
        if (timer->esp_timer != NULL) {
                esp_timer_stop(timer->esp_timer);
                esp_timer_delete(timer->esp_timer);
        }
        if (timer->timer_event_fired_handler != NULL) {
                esp_event_handler_instance_unregister(SEYRUSEFER_TIMER_EVENTS, SEYRUSEFER_TIMER_EVENT_FIRED, timer->timer_event_fired_handler);
        }
        free(timer);
}

int seyrusefer_timer_start (struct seyrusefer_timer *timer)
{
        int rc;
        if (timer == NULL) {
                seyrusefer_errorf("timer is invalid");
                goto bail;
        }
        if (timer->enabled == 1) {
                seyrusefer_errorf("timer is already enabled");
                goto out;
        }
        rc = esp_timer_start_periodic(timer->esp_timer,
                (timer->resolution == SEYRUSEFER_TIMER_RESOLUTION_SECONDS) ? 1000000 :
                (timer->resolution == SEYRUSEFER_TIMER_RESOLUTION_500_MILLISECONDS) ? 500000 :
                (timer->resolution == SEYRUSEFER_TIMER_RESOLUTION_250_MILLISECONDS) ? 250000 :
                (timer->resolution == SEYRUSEFER_TIMER_RESOLUTION_100_MILLISECONDS) ? 100000 :
                (timer->resolution == SEYRUSEFER_TIMER_RESOLUTION_50_MILLISECONDS) ? 50000 :
                (timer->resolution == SEYRUSEFER_TIMER_RESOLUTION_25_MILLISECONDS) ? 25000 :
                (timer->resolution == SEYRUSEFER_TIMER_RESOLUTION_MILLISECONDS) ? 1000 :
                (timer->resolution == SEYRUSEFER_TIMER_RESOLUTION_MICROSECONDS) ? 1 :
                (timer->resolution == SEYRUSEFER_TIMER_RESOLUTION_NANOSECONDS) ? 1 :
                100000);
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not start esp_timer");
                goto bail;
        }
        timer->enabled = 1;
out:    return 0;
bail:   return -1;
}

int seyrusefer_timer_stop (struct seyrusefer_timer *timer)
{
        int rc;
        if (timer == NULL) {
                seyrusefer_errorf("timer is invalid");
                goto bail;
        }
        if (timer->enabled == 0) {
                seyrusefer_errorf("timer is already stopped");
                goto out;
        }
        rc = esp_timer_stop(timer->esp_timer);
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not stop esp_timer");
                goto bail;
        }
        timer->enabled = 0;
out:    return 0;
bail:   return -1;
}

int64_t seyrusefer_timer_get_time (struct seyrusefer_timer *timer)
{
        if (timer == NULL) {
                seyrusefer_errorf("timer is invalid");
                goto bail;
        }
        return esp_timer_get_time();
bail:   return -1;
}

int seyrusefer_alarm_init_options_default (struct seyrusefer_alarm_init_options *options)
{
        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        memset(options, 0, sizeof(struct seyrusefer_alarm_init_options));
        return 0;
bail:   return -1;
}

struct seyrusefer_alarm * seyrusefer_alarm_create (struct seyrusefer_alarm_init_options *options)
{
        struct seyrusefer_alarm *alarm;

        alarm = NULL;

        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        if (options->timer == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }

        alarm = malloc(sizeof(struct seyrusefer_alarm));
        if (alarm == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(alarm, 0, sizeof(struct seyrusefer_alarm));

        alarm->callback_fired   = options->callback_fired;
        alarm->callback_context = options->callback_context;
        alarm->timer            = options->timer;
        alarm->timeout          = options->timeout;
        alarm->singleshot       = options->singleshot;
        alarm->enabled          = options->enabled;
        alarm->firetime         = esp_timer_get_time() + alarm->timeout;

        TAILQ_INSERT_TAIL(&alarm->timer->alarms, alarm, tailq);

        return alarm;
bail:   if (alarm != NULL) {
                seyrusefer_alarm_destroy(alarm);
        }
        return NULL;
}

void seyrusefer_alarm_destroy (struct seyrusefer_alarm *alarm)
{
        if (alarm == NULL) {
                return;
        }
        free(alarm);
}

int seyrusefer_alarm_start (struct seyrusefer_alarm *alarm)
{
        return seyrusefer_alarm_set_enabled(alarm, 1);
}

int seyrusefer_alarm_stop (struct seyrusefer_alarm *alarm)
{
        return seyrusefer_alarm_set_enabled(alarm, 0);
}

int seyrusefer_alarm_get_timeout (struct seyrusefer_alarm *alarm)
{
        if (alarm == NULL) {
                seyrusefer_errorf("alarm is invalid");
                goto bail;
        }
        return alarm->timeout;
bail:   return -1;
}

int seyrusefer_alarm_set_timeout (struct seyrusefer_alarm *alarm, int timeout)
{
        if (alarm == NULL) {
                seyrusefer_errorf("alarm is invalid");
                goto bail;
        }
        alarm->timeout  = timeout;
        alarm->firetime = esp_timer_get_time() + alarm->timeout;
        return 0;
bail:   return -1;
}

int seyrusefer_alarm_set_singleshot (struct seyrusefer_alarm *alarm, int singleshot)
{
        if (alarm == NULL) {
                seyrusefer_errorf("alarm is invalid");
                goto bail;
        }
        alarm->singleshot = singleshot;
        return 0;
bail:   return -1;
}

int seyrusefer_alarm_set_enabled (struct seyrusefer_alarm *alarm, int enabled)
{
        if (alarm == NULL) {
                seyrusefer_errorf("alarm is invalid");
                goto bail;
        }
        alarm->enabled  = enabled;
        alarm->firetime = esp_timer_get_time() + alarm->timeout;
        return 0;
bail:   return -1;
}
