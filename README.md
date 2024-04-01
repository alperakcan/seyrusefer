
# seyrusefer

Controlling unit designed for adventure bikes.

Controlling unit is a Bluetooth remote device that lets you to operate
various applications from handlebar without any need to touch the display.

This repository contains the software, designed by Alper Akcan, to run the
hardware controller project, also known as Seyrusefer, designed by Sefa Dogan.

Hardware design files and DIY building instructions are available in the
"docs" directory.

# 1. build & flash

ESP-IDF is a great software development environment for the hardware based on
the ESP32 chip by Espressif. Please follow the ESP-IDF Programming Guide at
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/ if you
are not familiar with ESP-IDF.

## 1.1. debian

### 1.1.1. prerequists

    sudo apt-get install \
        git \
        wget \
        flex \
        bison \
        gperf \
        python3 \
        python3-pip \
        python3-setuptools \
        python3-venv \
        cmake \
        ninja-build \
        ccache \
        libffi-dev \
        libssl-dev \
        dfu-util \
        libusb-1.0-0

### 1.1.2. /dev/ttyUSB0 permission

    sudo usermod -a -G dialout $USER

### 1.1.3. install esp-idf

### 1.1.3.1. clone

    git clone -b v5.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v5.2.git

### 1.1.3.2. setup tools

    cd esp-idf-v5.2.git
    ./install.sh esp32

### 1.1.3.3. setup environment

    cd esp-idf-v5.2.git
    . export.sh

### 1.1.3.4. hello world

    cp -r $IDF_PATH/examples/get-started/hello_world esp-idf-example-hello_world
    cd esp-idf-example-hello_world
    idf.py set-target esp32
    idf.py menuconfig
    idf.py build
    idf.py -p /dev/ttyUSB0 flash
    idf.py -p /dev/ttyUSB0 monitor

### 1.1.4 install application

    idf.py set-target esp32
    idf.py menuconfig
    idf.py build
    idf.py -p /dev/ttyUSB0 erase_flash
    idf.py -p /dev/ttyUSB0 flash monitor

### 1.1.4 build & flash merged application

    idf.py set-target esp32
    idf.py menuconfig
    idf.py build

    ( \
        cd build && \
        esptool.py --chip esp32 merge_bin -o ../seyrusefer-esp32-v1.0.2.bin @flash_args \
    )

    esptool.py -p /dev/ttyUSB0 erase_flash
    esptool.py -p /dev/ttyUSB0 write_flash 0x0 seyrusefer-esp32-v1.0.2.bin

    idf.py -p /dev/ttyUSB0 monitor

# 2. flash

Please downlaod latest bin file from https://github.com/alperakcan/seyrusefer/releases
and flash.

## 2.1 windows

Please download and install USB to UART Bridge Controller driver from
https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads for CP210x, and
https://wch-ic.com/search?q=CH340&t=downloads for CH34x.

Please download and approtiate version for windows (ex: esptool-v4.7.0-win64.zip)
from https://github.com/espressif/esptool/releases

Select file as seyrusefer.bin and offset as 0x0000.

    ./esptool.exe erase_flash
    ./esptool.exe write_flash 0x0 seyrusefer-esp32-v1.0.2.bin

# 3. run

Button combinations (press and hold for 2.5 seconds):
  - 1 & 2 : mode selection

    Select mode (press once):
      - 1 : mode 1
      - 2 : mode 2
      - 3 : mode 3
      - 4 : mode 4
      - 5 : mode 5

  - 1 & 3 : wifi setup
  - 1 & 4 : restart
  - 1 & 5 : restore to defaults
