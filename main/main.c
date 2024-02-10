
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_task_wdt.h"

#define SEYRUSEFER_DEBUG_TAG "main"
#include "debug.h"
#include "platform.h"
#include "seyrusefer.h"

void app_main (void)
{
        int rc;

        int iteration;
        struct seyrusefer *seyrusefer;
        struct seyrusefer_init_options seyrusefer_init_options;

        seyrusefer = NULL;

        seyrusefer_infof("initializing platform");
        rc = seyrusefer_platform_init();
        if (rc < 0) {
                seyrusefer_errorf("can not init seyrusefer platform");
                goto bail;
        }

        seyrusefer_infof("creating seyrusefer");
        rc = seyrusefer_init_options_default(&seyrusefer_init_options);
        if (rc < 0) {
                seyrusefer_errorf("can not init seyrusefer init options");
                goto bail;
        }
        seyrusefer = seyrusefer_create(&seyrusefer_init_options);
        if (seyrusefer == NULL) {
                seyrusefer_errorf("can not create seyrusefer");
                goto bail;
        }
        iteration = seyrusefer_get_iteration(seyrusefer);

        esp_task_wdt_add(NULL);
        esp_task_wdt_status(NULL);
        while (1) {
                int i;
                i = seyrusefer_get_iteration(seyrusefer);
                if (i != iteration) {
                        iteration = i;
                        esp_task_wdt_reset();
                }
                vTaskDelay(pdMS_TO_TICKS(250));
        }

        return;
bail:   seyrusefer_errorf("error occured, restarting");
        if (seyrusefer != NULL) {
                seyrusefer_destroy(seyrusefer);
        }
        seyrusefer_platform_deinit();
        seyrusefer_infof("Restarting in 10 seconds...");
        vTaskDelay(pdMS_TO_TICKS(10000));
        esp_restart();
}
