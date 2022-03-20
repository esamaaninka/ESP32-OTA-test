# ESP32-OTA-test
practising with ESP32 OTA 
https://randomnerdtutorials.com/esp32-ota-over-the-air-vs-code/

Chip is ESP32-D0WDQ6 (revision 1)
Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding 
Scheme None
Crystal is 40MHz
MAC: c4:4f:33:65:dc:a

Notes:
- after build, browse to the ESP32 webpage http://192.168.1.110/update and load the firmware.bin from
project C:\Users\ett18304\Documents\coding\ESP32-OTA-test\ESP32-OTA-test\.pio\build\esp32doit-devkit-v1\firmware.bin
Note! with McUbuntu the hidden folders not visible with the browser upload.
- NOTE! If the code you update crashes, there is no way to update it anymore over OTA! 
=> working Serial connection is a must!
(Updated CP210x driver did not help, but when changed the physical USB cable PC recognized the ESP32 again!!!)
JOTAIN MENI PIELEEN WEBSERIAL EI ENÄÄ TOIMINUT, KOMMENTOIN VÄLIAIKAISESTI POIS


MQTT according to https://www.emqx.com/en/blog/esp32-connects-to-the-free-public-mqtt-broker
now subscribe to the broker, sends greeting and start to listen
broker with IP address 
port 1883
topic esp32/test

NOTE!! If mqtt register fail, the app is not working!

WLAN Newtwork  currently only Pajaranta

SPIFFS for web pages added. KOKEILE latautuuko /data oleva index.html jossa napit
https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/
lataa filet ESP32 s.e Pio project, Platform, Build filesystem ja Upload filesystem


API
/               page toggle button to turn on/off internal led

/update         page to load updated firmware

/dht            page to shows graphical temp/hum gauges
/temperature    returns temp as string
/humidity       returns humidity as string

/webserial      page with serial console in browser
https://github.com/ayushsharma82/WebSerial

/scale          returns HX711 reading as String
https://registry.platformio.org/libraries/bogde/HX711/examples/HX711_basic_example/HX711_basic_example.ino


My Generic Notes about ESP32 development.

VSC Build layer? does not work

Serial Monitor and build upload:
- EttePC and EsaWinkkari neither one serial build upload does not work.
- EsaLinukka worked ok.

- McUbuntu build upload works with serial
- VSC serial monitor does not work, 
    - sometimes got it to work with external /opt/CoolTermLinux/CoolTerm 


VSC with Arduino extensions:
(c++, programmer: AVR ISP, board: DOIT ESP32 DEVKIT V1,/dev/ttyUSB0 )



