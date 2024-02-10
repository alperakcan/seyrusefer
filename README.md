
# seyrusefer

Controlling unit designed especially for adventure bikes.

# 1. build

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

### 1.1.3. install espressif

### 1.1.3.1. clone

    git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v5.1.2.git

### 1.1.3.2. setup tools

    cd esp-idf-v5.1.2.git
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
