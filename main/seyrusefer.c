
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"

#define SEYRUSEFER_DEBUG_TAG "seyrusefer"
#include "debug.h"
#include "timer.h"
#include "seyrusefer.h"

#define SEYRUSEFER_VERSION                              "seyrusefer-0.0.0.bin"

enum {
        SEYRUSEFER_STATE_NONE,
        SEYRUSEFER_STATE_INIT,
        SEYRUSEFER_STATE_RUNNING,
        SEYRUSEFER_STATE_ERROR
#define SEYRUSEFER_STATE_NONE                           SEYRUSEFER_STATE_NONE
#define SEYRUSEFER_STATE_INIT                           SEYRUSEFER_STATE_INIT
#define SEYRUSEFER_STATE_RUNNING                        SEYRUSEFER_STATE_RUNNING
#define SEYRUSEFER_STATE_ERROR                          SEYRUSEFER_STATE_ERROR
};

struct seyrusefer {
        int state;
        int pstate;
        int restart;

        struct seyrusefer_timer *timer;

        struct seyrusefer_alarm *process_alarm;
};

static const char * seyrusefer_state_string (int state);
static int seyrusefer_set_state_init_actual (struct seyrusefer *seyrusefer);
static int seyrusefer_set_state_running_actual (struct seyrusefer *seyrusefer);
static int seyrusefer_set_state_error_actual (struct seyrusefer *seyrusefer);
static int seyrusefer_set_state (struct seyrusefer *seyrusefer, int state);

static void process_alarm_callback_fired (struct seyrusefer_alarm *alarm, void *context);

static const char * seyrusefer_state_string (int state)
{
        if (state == SEYRUSEFER_STATE_NONE)             return "none";
        if (state == SEYRUSEFER_STATE_INIT)             return "init";
        if (state == SEYRUSEFER_STATE_RUNNING)          return "running";
        if (state == SEYRUSEFER_STATE_ERROR)            return "error";
        return "unknown";
}

static int seyrusefer_set_state_init_actual (struct seyrusefer *seyrusefer)
{
        int rc;
        seyrusefer_infof("starting timer");
        rc = seyrusefer_timer_start(seyrusefer->timer);
        if (rc < 0) {
                seyrusefer_errorf("can not start timer");
                goto bail;
        }
        seyrusefer_infof("enabling process alarm");
        rc = seyrusefer_alarm_set_enabled(seyrusefer->process_alarm, 1);
        if (rc < 0) {
                seyrusefer_errorf("can not enable process_alarm");
                goto bail;
        }
        rc = seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_RUNNING);
        if (rc < 0) {
                seyrusefer_errorf("can not set state to rogue");
                goto bail;
        }
        return 0;
bail:   return -1;
}

static int seyrusefer_set_state_running_actual (struct seyrusefer *seyrusefer)
{
        (void) seyrusefer;
        return 0;
}

static int seyrusefer_set_state_error_actual (struct seyrusefer *seyrusefer)
{
        seyrusefer_errorf("entered in error state, restarting...");
        seyrusefer->restart = 1;
        return 0;
}

static int seyrusefer_set_state (struct seyrusefer *seyrusefer, int state)
{
        int rc;

        if (seyrusefer == NULL) {
                seyrusefer_errorf("seyrusefer is invalid");
                goto bail;
        }

        if (seyrusefer->state == state) {
                seyrusefer_debugf("state is already set");
                goto out;
        }
        seyrusefer->pstate = seyrusefer->state;
        seyrusefer->state  = state;

        seyrusefer_infof("seyrusefer state: %d, %s, pstate: %d, %s", seyrusefer->state, seyrusefer_state_string(seyrusefer->state), seyrusefer->pstate, seyrusefer_state_string(seyrusefer->pstate));

        if (seyrusefer->state == SEYRUSEFER_STATE_INIT) {
                rc = seyrusefer_set_state_init_actual(seyrusefer);
                if (rc != 0) {
                        seyrusefer_errorf("seyrusefer_set_state_init_actual failed");
                        goto bail;
                }
        } else if (seyrusefer->state == SEYRUSEFER_STATE_RUNNING) {
                rc = seyrusefer_set_state_running_actual(seyrusefer);
                if (rc != 0) {
                        seyrusefer_errorf("seyrusefer_set_state_running_actual failed");
                        goto bail;
                }
        } else if (seyrusefer->state == SEYRUSEFER_STATE_ERROR) {
                rc = seyrusefer_set_state_error_actual(seyrusefer);
                if (rc != 0) {
                        seyrusefer_errorf("seyrusefer_set_state_error_actual failed");
                        goto bail;
                }
        } else {
                seyrusefer_errorf("state is invalid");
                goto bail;
        }

out:    return 0;
bail:   return -1;
}

static void process_alarm_callback_fired (struct seyrusefer_alarm *alarm, void *context)
{
        int rc;
        struct seyrusefer *seyrusefer = context;

        (void) alarm;

        seyrusefer_infof("process");

        rc = seyrusefer_process(seyrusefer);
        if (rc < 0) {
                seyrusefer_errorf("seyrusefer_process failed, rc: %d", rc);
                goto bail;
        }

        return;
bail:   seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_ERROR);
        return;
}

int seyrusefer_init_options_default (struct seyrusefer_init_options *options)
{
        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }
        memset(options, 0, sizeof(struct seyrusefer_init_options));
        return 0;
bail:   return -1;
}

struct seyrusefer * seyrusefer_create (struct seyrusefer_init_options *options)
{
        int rc;
        struct seyrusefer *seyrusefer;
        struct seyrusefer_timer_init_options seyrusefer_timer_init_options;
        struct seyrusefer_alarm_init_options seyrusefer_process_alarm_init_options;

        seyrusefer = NULL;

        if (options == NULL) {
                seyrusefer_errorf("options is invalid");
                goto bail;
        }

        seyrusefer = malloc(sizeof(struct seyrusefer));
        if (seyrusefer == NULL) {
                seyrusefer_errorf("can not allocate memory");
                goto bail;
        }
        memset(seyrusefer, 0, sizeof(struct seyrusefer));

        seyrusefer->state       = SEYRUSEFER_STATE_NONE;
        seyrusefer->pstate      = SEYRUSEFER_STATE_NONE;
        seyrusefer->restart     = 0;

        seyrusefer_infof("creating timer");
        seyrusefer_infof("  enabled   : %d", 0);
        seyrusefer_infof("  resolution: %s", "default");
        rc = seyrusefer_timer_init_options_default(&seyrusefer_timer_init_options);
        if (rc < 0) {
                seyrusefer_errorf("can not init timer init options");
                goto bail;
        }
        seyrusefer_timer_init_options.resolution = SEYRUSEFER_TIMER_RESOLUTION_100_MILLISECONDS;
        seyrusefer_timer_init_options.enabled    = 0;
        seyrusefer->timer = seyrusefer_timer_create(&seyrusefer_timer_init_options);
        if (seyrusefer->timer == NULL) {
                seyrusefer_errorf("can not create timer");
                goto bail;
        }

        seyrusefer_infof("creating process alarm");
        seyrusefer_infof("  enabled: %d", 0);
        rc = seyrusefer_alarm_init_options_default(&seyrusefer_process_alarm_init_options);
        if (rc < 0) {
                seyrusefer_errorf("can not init alarm init options");
                goto bail;
        }
        seyrusefer_process_alarm_init_options.callback_fired     = process_alarm_callback_fired;
        seyrusefer_process_alarm_init_options.callback_context   = seyrusefer;
        seyrusefer_process_alarm_init_options.timer              = seyrusefer->timer;
        seyrusefer_process_alarm_init_options.timeout            = 1000000;
        seyrusefer_process_alarm_init_options.singleshot         = 0;
        seyrusefer_process_alarm_init_options.enabled            = 0;
        seyrusefer->process_alarm = seyrusefer_alarm_create(&seyrusefer_process_alarm_init_options);
        if (seyrusefer->process_alarm == NULL) {
                seyrusefer_errorf("can not create process alarm");
                goto bail;
        }

        {
	        const esp_partition_t *running_partition;

		running_partition = esp_ota_get_running_partition();

                seyrusefer_infof("running_partition: %p", running_partition);
                seyrusefer_infof("  label  : %s", running_partition->label);
                seyrusefer_infof("  type   : %d", running_partition->type);
                seyrusefer_infof("  subtype: %d", running_partition->subtype);
        }

        rc = seyrusefer_set_state(seyrusefer, SEYRUSEFER_STATE_INIT);
        if (rc != 0) {
                seyrusefer_errorf("can not set seyrusefer state to init");
                goto bail;
        }

        return seyrusefer;
bail:   if (seyrusefer != NULL) {
                seyrusefer_destroy(seyrusefer);
        }
        return NULL;
}

void seyrusefer_destroy (struct seyrusefer *seyrusefer)
{
        if (seyrusefer == NULL) {
                return;
        }
        if (seyrusefer->process_alarm != NULL) {
                seyrusefer_alarm_destroy(seyrusefer->process_alarm);
        }
        if (seyrusefer->timer != NULL) {
                seyrusefer_timer_destroy(seyrusefer->timer);
        }
        free(seyrusefer);
}

int seyrusefer_process (struct seyrusefer *seyrusefer)
{
        if (seyrusefer->restart) {
                seyrusefer_errorf("restart requested, in 1 second");
                vTaskDelay(pdMS_TO_TICKS(1000));
                esp_restart();
                return 0;
        }
        return 0;
}
