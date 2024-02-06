
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_event.h"

#define SEYRUSEFER_DEBUG_TAG "main"
#include "debug.h"
#include "platform.h"
#include "seyrusefer.h"

void app_main (void)
{
        int rc;

        struct seyrusefer *seyrusefer;
        struct seyrusefer_init_options seyrusefer_init_options;

        seyrusefer = NULL;

        rc = esp_event_loop_create_default();
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not create event loop");
                goto bail;
        }

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

        return;
bail:   seyrusefer_errorf("error occured, restarting");
        if (seyrusefer != NULL) {
                seyrusefer_destroy(seyrusefer);
        }
        seyrusefer_platform_deinit();
        esp_event_loop_delete_default();
        seyrusefer_infof("Restarting in 10 seconds...");
        vTaskDelay(pdMS_TO_TICKS(10000));
        esp_restart();
}
