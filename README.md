
# seyrusefer

Controlling unit designed for adventure bikes.

Controlling unit is a Bluetooth remote devices that lets you to operate
various applications from handlebar without the need of touching the display.

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

    cd esp-idf-v5.1.2.git
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
        esptool.py --chip esp32 merge_bin -o ../seyrusefer-esp32-v1.0.0.bin @flash_args \
    )

    esptool.py -p /dev/ttyUSB0 erase_flash
    esptool.py -p /dev/ttyUSB0 write_flash 0x0 seyrusefer-esp32-v1.0.0.bin

    idf.py -p /dev/ttyUSB0 monitor

# 2. flash

Please downlaod latest bin file from https://github.com/alperakcan/seyrusefer/releases
and flash.

## 2.1 windows

Please download and install USB to UART Bridge Controller driver from
https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads

### 2.1.2 flash download tools

Please download and install Flash Download Tools from ESPRESSIF from
https://www.espressif.com/en/support/download/other-tools

Select file as seyrusefer.bin and offset as 0x0000.

## 2.2 esptool-js

This repository contains a Javascript implementation of esptool, a serial flasher
utility for Espressif chips. esptool-js is based on Web Serial API and works in
Google Chrome and Microsoft Edge version 89 or later browsers.

Please visit https://espressif.github.io/esptool-js/ and follow instructions
to program your device.

Select file as seyrusefer.bin and offset as 0x0000.

## 2.3 esp web tool

ESP Web Tool is a open source web app project to flash the device, please see
project details at https://github.com/SpacehuhnTech/espwebtool and use it at
your own risk.

Please visit https://esp.huhn.me/ and follow instructions to program your
device.

Select file as seyrusefer.bin and offset as 0x0000.
