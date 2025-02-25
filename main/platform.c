
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#define SEYRUSEFER_DEBUG_TAG "platform"
#include "debug.h"
#include "platform.h"

#if defined(CONFIG_IDF_TARGET_ESP32) && (CONFIG_IDF_TARGET_ESP32 == 1)

/* push buttons */
const int GPIO_BUTTON_1 = GPIO_NUM_14;
const int GPIO_BUTTON_2 = GPIO_NUM_27;
const int GPIO_BUTTON_3 = GPIO_NUM_26;

/* toggle switch */
const int GPIO_BUTTON_4 = GPIO_NUM_25;
const int GPIO_BUTTON_5 = GPIO_NUM_12;

/* led */
const int GPIO_LED      = GPIO_NUM_13;

#elif defined(CONFIG_IDF_TARGET_ESP32C3) && (CONFIG_IDF_TARGET_ESP32C3 == 1)

/* push buttons */
const int GPIO_BUTTON_1 = GPIO_NUM_3;
const int GPIO_BUTTON_2 = GPIO_NUM_2;
const int GPIO_BUTTON_3 = GPIO_NUM_1;

/* toggle switch */
const int GPIO_BUTTON_4 = GPIO_NUM_10;
const int GPIO_BUTTON_5 = GPIO_NUM_20;

/* led */
const int GPIO_LED      = GPIO_NUM_9;

#endif

static int gpio_init (void)
{
        int rc;

        rc  = 0;

        rc |= gpio_reset_pin(GPIO_BUTTON_1);
        rc |= gpio_reset_pin(GPIO_BUTTON_2);
        rc |= gpio_reset_pin(GPIO_BUTTON_3);
        rc |= gpio_reset_pin(GPIO_BUTTON_4);
        rc |= gpio_reset_pin(GPIO_BUTTON_5);

	rc |= gpio_set_direction(GPIO_BUTTON_1, GPIO_MODE_INPUT);
	rc |= gpio_set_direction(GPIO_BUTTON_2, GPIO_MODE_INPUT);
	rc |= gpio_set_direction(GPIO_BUTTON_3, GPIO_MODE_INPUT);
	rc |= gpio_set_direction(GPIO_BUTTON_4, GPIO_MODE_INPUT);
	rc |= gpio_set_direction(GPIO_BUTTON_5, GPIO_MODE_INPUT);

        rc |= gpio_set_pull_mode(GPIO_BUTTON_1, GPIO_PULLUP_ONLY);
        rc |= gpio_set_pull_mode(GPIO_BUTTON_2, GPIO_PULLUP_ONLY);
        rc |= gpio_set_pull_mode(GPIO_BUTTON_3, GPIO_PULLUP_ONLY);
        rc |= gpio_set_pull_mode(GPIO_BUTTON_4, GPIO_PULLUP_ONLY);
        rc |= gpio_set_pull_mode(GPIO_BUTTON_5, GPIO_PULLUP_ONLY);

        //rc |= gpio_reset_pin(GPIO_LED);
	rc |= gpio_set_direction(GPIO_LED, GPIO_MODE_OUTPUT);

        #define LEDC_TIMER              LEDC_TIMER_0
        #define LEDC_MODE               LEDC_LOW_SPEED_MODE
        #define LEDC_OUTPUT_IO          GPIO_LED
        #define LEDC_CHANNEL            LEDC_CHANNEL_0
        #define LEDC_DUTY_RES           LEDC_TIMER_14_BIT
        #define LEDC_DUTY               (1 << 14)
        #define LEDC_FREQUENCY          (4000)

        ledc_timer_config_t ledc_timer = {
                .speed_mode       = LEDC_MODE,
                .duty_resolution  = LEDC_DUTY_RES,
                .timer_num        = LEDC_TIMER,
                .freq_hz          = LEDC_FREQUENCY,
                .clk_cfg          = LEDC_AUTO_CLK
        };
        rc |= ledc_timer_config(&ledc_timer);

        ledc_channel_config_t ledc_channel = {
                .speed_mode     = LEDC_MODE,
                .channel        = LEDC_CHANNEL,
                .timer_sel      = LEDC_TIMER,
                .intr_type      = LEDC_INTR_DISABLE,
                .gpio_num       = LEDC_OUTPUT_IO,
                .duty           = 0,
                .hpoint         = 0
        };
        rc |= ledc_channel_config(&ledc_channel);

        if (rc != 0) {
                seyrusefer_errorf("gpi setup failed");
                goto bail;
        }
        return 0;
bail:   return -1;
}

int seyrusefer_platform_init (void)
{
        int rc;

        rc = gpio_init();
        if (rc < 0) {
                seyrusefer_errorf("can not int gpio");
                goto bail;
        }
        rc = nvs_flash_init();
        if (rc == ESP_ERR_NVS_NO_FREE_PAGES ||
            rc == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                rc = nvs_flash_erase();
                if (rc != ESP_OK) {
                        seyrusefer_errorf("can not erase flash");
                        goto bail;
                }
                rc = nvs_flash_init();
        }
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not create nvs flash");
                goto bail;
        }

        rc = nvs_flash_init_partition("config");
        if (rc == ESP_ERR_NVS_NO_FREE_PAGES ||
            rc == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                rc = nvs_flash_erase_partition("config");
                if (rc != ESP_OK) {
                        seyrusefer_errorf("can not erase flash");
                        goto bail;
                }
                rc = nvs_flash_init_partition("config");
        }
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not create nvs flash");
                goto bail;
        }

        rc = esp_netif_init();
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not create netif");
                goto bail;
        }

        rc = esp_event_loop_create_default();
        if (rc != ESP_OK) {
                seyrusefer_errorf("can not create event loop");
                goto bail;
        }

        return 0;
bail:   return -1;
}

int seyrusefer_platform_deinit (void)
{
        esp_netif_deinit();
        esp_event_loop_delete_default();
        nvs_flash_deinit();
        return 0;
}

int seyrusefer_platform_restart (void)
{
        esp_restart();
        return 0;
}

int seyrusefer_platform_get_buttons (void)
{
        int i;
        int l;
        int b;
        struct {
                int gpio;
                int button;
        } gpios[] = {
                { GPIO_BUTTON_1, SEYRUSEFER_PLATFORM_BUTTON_1 },
                { GPIO_BUTTON_2, SEYRUSEFER_PLATFORM_BUTTON_2 },
                { GPIO_BUTTON_3, SEYRUSEFER_PLATFORM_BUTTON_3 },
                { GPIO_BUTTON_4, SEYRUSEFER_PLATFORM_BUTTON_4 },
                { GPIO_BUTTON_5, SEYRUSEFER_PLATFORM_BUTTON_5 },
        };

        b = 0;
        for (i = 0; i < (sizeof(gpios) / sizeof(gpios[0])); i++) {
                l = gpio_get_level(gpios[i].gpio);
                if (l == 0) {
                        b |= gpios[i].button;
                }
        }

        return b;
}

int seyrusefer_platform_set_led (int on)
{
        int rc;
        ledc_channel_config_t ledc_channel = {
                .speed_mode     = LEDC_MODE,
                .channel        = LEDC_CHANNEL,
                .timer_sel      = LEDC_TIMER,
                .intr_type      = LEDC_INTR_DISABLE,
                .gpio_num       = LEDC_OUTPUT_IO,
                .duty           = (LEDC_DUTY * on) / 100,
                .hpoint         = 0
        };
        rc = ledc_channel_config(&ledc_channel);
        if (rc != 0) {
                seyrusefer_errorf("can not set led to: %d", on);
                goto bail;
        }
        return 0;
bail:   return -1;
}
