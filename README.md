Simple mp3player for the Odroid Go (ESP32)

Installing of ESP32-IDF/FreeRTOS (on Ubuntu Linux):
(fetched on 20.08.2019)
```
sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache
git clone --recursive https://github.com/espressif/esp-idf.git
sudo ./esp-idf/install.sh
```

Making and Flashing commands used (on Ubuntu Linux): 
```
. ./esp-idf/export.sh
cd mp3player
make menuconfig
-> Serial flasher config
-> SAVE (file is 'sdkconfig')
make (not needed if you've generated a new sdkconfig, but doesn't hurt...)
make flash (turn Odroid on before...)
```

Compiling with fresh cloning, and without anything (apart from esptool (pip3 install esptool)) on your flashing computer:
```
sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache
mkdir mp3player
cd mp3player
git clone https://github.com/ThomasChr/odroidgo_mp3player .
git clone --recursive https://github.com/espressif/esp-idf.git
sudo ./esp-idf/install.sh
. ./esp-idf/export.sh
cd mp3player
make clean
make
```
-> Grab the files '/build/bootloader/bootloader.bin', '/build/mp3player.bin' and '/build/partitions_singleapp.bin'

On the flashing computer:
```
pip3 install esptool
esptool.py --chip esp32 --port COM41 erase_flash
esptool.py --chip esp32 --port COM41 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 c:\temp\bootloader.bin 0x10000 c:\temp\mp3player.bin 0x8000 c:\temp\partitions_singleapp.bin
(for Linux Port is something like '/dev/ttyUSB0')
```
