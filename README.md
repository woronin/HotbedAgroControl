# HotbedAgroControl

Контроллер для управления гидропонной установкой, который управляет реле, измеряет температуру, влажность воздуха, температуру раствора, уровень воды в баке, измеряет кислотно-щелочной баланс и электропроводность водного раствора, а также измеряет освещенность падающего света на листья растений.

Все исходные коды проекта опубликованы по адресу:  
https://github.com/woronin/HotbedAgroControl

Телеграм канал поддержки проекта:  
https://t.me/hotbed_agro

## Команды для загрузки проекта в контроллер ESP32

```bash
cd "Ваша директория проекта где расположен файл platformio.ini"
pio run
cd .pio/build/esp32dev/
esptool --chip esp32 --port /dev/ttyACM0 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin
