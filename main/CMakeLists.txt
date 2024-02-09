
idf_component_register(
        SRCS
                "hid_dev.c"
                "hid_device_le_prf.c"
                "hid_hidd_prf_api.c"
                "main.c"
                "seyrusefer.c"
                "debug.c"
                "config.c"
                "hid.c"
                "httpd.c"
                "platform.c"
                "settings.c"
                "timer.c"
                "uuid.c"
                "wifi.c"

        INCLUDE_DIRS
                "."

        EMBED_FILES
                "www/index.html"
                "www/bootstrap-icons.css"
                "www/bootstrap.min.css"
                "www/bootstrap.min.js"
                "www/jquery.min.js"
                "www/popper.min.js"
)
