# ESP32-OTA-test
practising with ESP32 OTA 
https://randomnerdtutorials.com/esp32-ota-over-the-air-vs-code/

Notes:
- after build, browse to the ESP32 webpage http://192.168.1.110/update and load the firmware.bin from
project C:\Users\ett18304\Documents\coding\ESP32-OTA-test\ESP32-OTA-test\.pio\build\esp32doit-devkit-v1\firmware.bin
Note! with McUbuntu the hidden folders not visible with the browser upload.

WLAN Newtwork  currently only Pajaranta


API
/   toggle button to turn on/off internal led

/update     page to load updated firmware

/dht shows graphical temp/hum page
/temperature  returns temp as text
/humidity       returns humidity as text

/webserial      opens serial console in browser
https://github.com/ayushsharma82/WebSerial

/scale      returns HX711 reading as String
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

[Warning] Output path is not specified. Unable to reuse previously compiled files. Verify could be slow.
-> vaikka laitoin .vscode/arduino.json output parametriksi "../build" ei toimi