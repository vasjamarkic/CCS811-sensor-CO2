# CCS811-sensor-CO2
Measuring CO2 and tVOC with Arduino

© 2020 GitHub, Inc.
How to use the CO2 sensor and program it:
https://create.arduino.cc/projecthub/dragos-iosub/arduino-ccs811-co2-tvoc-air-quality-monitor-570af1

Asynhronous web server (example for Temp. and Humidity measuring): https://www.youtube.com/watch?v=tDdL5urWvH4 and 
https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/

![alt text](https://raw.githubusercontent.com/espressif/arduino-esp32/master/docs/esp32_pinmap.png)

how to make ESP32 work in Linux and Arduino IDE:
Reffer to: https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/debian_ubuntu.md
1. install Arduino IDE 1.8 or greater: https://www.arduino.cc/en/Guide/Linux
2. Start Arduino IDE, File --> Preferences and add this URL link: https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json, click OK.
3. Tools --> Boards --> Boards Manager ... --> write down in filter ESP32 and pick it up --> install!
4. In Tools --> Boards --> choose your own ESP32! 
CCS811 wiring and programming:
5. D22 ~ SCL, D21 ~ SDA, GND ~ GND, VIN ~ VCC, GND ~ WAKE (other PINS in the sensors are NC)
6. Add the sSense-CCS811.h library
7. Copy paste the code from here : https://create.arduino.cc/projecthub/dragos-iosub/arduino-ccs811-co2-tvoc-air-quality-monitor-570af1
8. Verify, Upload, RESET the ESP32 board with the BOOT button, the measurments should now begin to appear. Run Serial Monitor.
WiFi connection ESP32:
9. Add the WiFi.h library
10. Write the code: https://techtutorialsx.com/2017/04/24/esp32-connecting-to-a-wifi-network/
ASYNHRONOUS web server:
The tutorial HERE: https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/
11. Get the library here: https://github.com/me-no-dev/ESPAsyncWebServer or download de ZIP file: https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip
12. Unzip the .zip folder and you should get ESPAsyncWebServer-master folder
13. Rename your folder from ESPAsyncWebServer-master to ESPAsyncWebServer
14. Move the ESPAsyncWebServer folder to your Arduino IDE installation libraries folder
The ESPAsyncWebServer library requires the AsyncTCP library to work. Follow the next steps to install that library:
15. Click here https://github.com/me-no-dev/AsyncTCP/archive/master.zip to download the AsyncTCP library. 
    You should have a .zip folder in your Downloads folder
16. Unzip the .zip folder and you should get AsyncTCP-master folder
17. Rename your folder from AsyncTCP-master to AsyncTCP
18. Move the AsyncTCP folder to your Arduino IDE installation libraries folder
19. Finally, re-open your Arduino IDE
20. The #include is <AsyncTCP.h> !
21. Wiring OLED (SPI protocol) with ESP32:
    Pinout connections:
    GND - GND
    Vcc - 3.3V
    D0 (MISO) - GPIO19
    D1 (MOSI) - GPIO23
    RES - GPIO4
    DC - GPIO18
    CS - GPIO5
Install SPIFFS (in Linux, for Win, skip this):
22. for installing in Arduino IDE  the ESP SPIFFS plugin reffer here:  https://www.onetransistor.eu/2019/12/upload-files-esp8266-esp32-spiffs.html 
    Copy ESP32FS (find here: https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/tag/1.0) in a folder tools! (create the folder!). Restart Arduino IDE. 
Creating the web page:
23. Create charts in HTML, reffer here: https://randomnerdtutorials.com/esp32-esp8266-plot-chart-web-server/
    Create /data folder in arduino project folder and copy the index.html file from the repository
    Then upload the data sketch (index.html) via Tools --> ESP32 Sketch Data Upload!
24. More info how to change the charts (for static y-axis) in HTML: https://api.highcharts.com/highcharts/yAxis.min
