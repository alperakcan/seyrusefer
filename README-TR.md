# seyrusefer

Macera motosikletleri için tasarlanmış kontrol ünitesi.

Kontrol ünitesi, ekrana dokunmadan çeşitli uygulamaları gidon üzerinden
kullanmanızı sağlayan bir Bluetooth uzaktan kumandadır.

Bu depo, Sefa Doğan tarafından tasarlanmış olan Seyrusefer kontrol cihazı
projesini çalıştırmak için Alper Akcan tarafından tasarlanmış olan yazılımı
içerir.

Donanım tasarım dosyaları ve kendin yap talimatları "docs" dizininde mevcuttur.

# 1. derle & yükle

ESP-IDF, Espressif'in ESP32 çipine dayalı donanım için harika bir yazılım geliştirme ortamıdır.
ESP-IDF'ye aşina değilseniz lütfen ESP-IDF Programlama Kılavuzu'nu takip edin
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/

## 1.1. debian

### 1.1.1. ön koşullar

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

### 1.1.2. /dev/ttyUSB0 izni

    sudo usermod -a -G dialout $USER

### 1.1.3. esp-idf kurulumu

### 1.1.3.1. klon

    git clone -b v5.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v5.2.git

### 1.1.3.2. kurulum araçları

    cd esp-idf-v5.2.git
    ./install.sh esp32

### 1.1.3.3. ortam kurulumu

    cd esp-idf-v5.2.git
    . export.sh

### 1.1.3.4. merhaba dünya

    cp -r $IDF_PATH/examples/get-started/hello_world esp-idf-example-hello_world
    cd esp-idf-example-hello_world
    idf.py set-target esp32
    idf.py menuconfig
    idf.py build
    idf.py -p /dev/ttyUSB0 flash
    idf.py -p /dev/ttyUSB0 monitor

### 1.1.4 uygulama kurulumu

    idf.py set-target esp32
    idf.py menuconfig
    idf.py build
    idf.py -p /dev/ttyUSB0 erase_flash
    idf.py -p /dev/ttyUSB0 flash monitor

### 1.1.4 derle & yükle birleştirilmiş uygulama

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

# 2. flaş

Lütfen https://github.com/alperakcan/seyrusefer/releases adresinden
en son bin dosyasını indirin ve flaşlayın.

## 2.1. windows

Lütfen CP210x icin https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads,
CH34x icin https://wch-ic.com/search?q=CH340&t=downloads
adresinden USB'den UART Köprü Denetleyicisi sürücüsünü indirip kurun.

Lütfen https://github.com/espressif/esptool/releases adresinden
uygun sürümü indirin (örneğin: esptool-v4.7.0-win64.zip)

Dosyayı seyrusefer.bin olarak seçin ve ofseti 0x0000 olarak ayarlayın.

    ./esptool.exe erase_flash
    ./esptool.exe write_flash 0x0 seyrusefer-esp32-v1.0.2.bin

# 3. çalıştır

Düğme kombinasyonları (2.5 saniye basılı tutun):
  - 1 & 2 : mod seçimi

    Mod seçimi (bir kez basın):
      - 1 : mod 1
      - 2 : mod 2
      - 3 : mod 3
      - 4 : mod 4
      - 5 : mod 5
  - 1 & 3 : wifi ayarı
  - 1 & 4 : yeniden başlatma
  - 1 & 5 : varsayılanlara geri yükleme
