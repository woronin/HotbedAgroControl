# Для прошивки ESP32 по воздуху WIFI нужно в WEB интерфейсе выбрать файл firmware.bin
# Для прошивки ESP32 по  шнурку USB нужно перейти в проекте в директорию build  и там использовать все файлы командой 
```bash
$  cd .pio/build/esp32dev/
$ esptool --chip esp32 --port /dev/ttyACM0 --baud 460800 \
--before default_reset --after hard_reset write_flash -z --flash_mode dio \
--flash_freq 40m --flash_size 4MB 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 \
firmware.bin
```
# или упрощенная команда
$ esptool  write_flash 0x00000 firmware.bin 

# Альтернативно можно прошивать по шнурку USB из директории где файл platformio.ini командой 
$ pio run --target upload && pio device monitor

