# CCS811-sensor-CO2
Measuring CO2 and tVOC with Arduino


Bought sensor here: https://www.ebay.com/itm/KEYESTUDIO-CCS811-CO2-Carbon-Dioxide-Temperature-Air-Quality-Sensor-for-Arduino/133127477135?_trkparms=aid%3D111001%26algo%3DREC.SEED%26ao%3D1%26asc%3D225074%26meid%3Df92177c013464b938a77a630f5ee24c9%26pid%3D100675%26rk%3D1%26rkt%3D15%26mehot%3Dpp%26sd%3D133127477135%26itm%3D133127477135%26pmt%3D0%26noa%3D1%26pg%3D2380057%26brand%3DArduino&_trksid=p2380057.c100675.m4236&_trkparms=pageci%3A0a4ae3a1-185c-11eb-b502-760968028e57%7Cparentrq%3A6a576af41750a45e4ef47c20ffebe708%7Ciid%3A1

Guide here: https://create.arduino.cc/projecthub/dragos-iosub/arduino-ccs811-co2-tvoc-air-quality-monitor-570af1

Getting data via BlueTooth to (HC-05/HC-06) Android phone : https://www.instructables.com/How-to-Receive-Arduino-Sensor-Data-on-Your-Android/

Getting data via WiFi (ESP8266 Wi-Fi Module) to website: https://circuitdigest.com/microcontroller-projects/sending-arduino-data-to-webpage

Use of Arduino IoT Cloud: https://create.arduino.cc/projecthub/133030/iot-cloud-getting-started-c93255, https://www.arduino.cc/en/IoT/HomePage, https://create.arduino.cc/iot/things/new

Asynhronous web server (Temp. and Humidity): https://www.youtube.com/watch?v=tDdL5urWvH4 and 
https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/

And some troubleshootig over http://www.martyncurrey.com/arduino-esp8266/ wifi module.
![alt text](https://raw.githubusercontent.com/AchimPieters/esp32-homekit-camera/master/Images/ESP32-30PIN-DEVBOARD.png)

-------------------how to make ESP32 work in Linux and Arduino IDE ---------------------------------------------------------
Reffer to: https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/debian_ubuntu.md
1. install Arduino IDE 1.8 or greater: https://www.arduino.cc/en/Guide/Linux
2. Start Arduino IDE, File --> Preferences and add this URL link: https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json, click OK.
3. Tools --> Boards --> Boards Manager ... --> write down in filter ESP32 and pick it up --> install!
4. In Tools --> Boards choose your own ESP32! 
-------------CCS811 wiring and programming ----------------------------------------------------------------------------------
5. D22 ~ SCL, D21 ~ SDA, GND ~ GND, VIN ~ VCC, GND ~ WAKE (other PINS in the sensors are NC)
6. Add the sSense-CCS811.h library
7. Copy paste the code from here : https://create.arduino.cc/projecthub/dragos-iosub/arduino-ccs811-co2-tvoc-air-quality-monitor-570af1
8. The SERIAL_SPEED must be the same in the Seral Monitor! (19200)
9. Verify, Upload, RESET the ESP32 board with the BOOT button, the measurments should now begin to appear. Run Serial Monitor.
------------WiFi connection ESP32---------------------------------------------------------------------------------------------
10. Add the WiFi.h library
11. Write the code: https://techtutorialsx.com/2017/04/24/esp32-connecting-to-a-wifi-network/

------------IoT-----------ASYNHRONOUS web server---------------------------------

The tutorial HERE: https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/

12. Get the library here: https://github.com/me-no-dev/ESPAsyncWebServer or download de ZIP file: https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip
13. Unzip the .zip folder and you should get ESPAsyncWebServer-master folder
14. Rename your folder from ESPAsyncWebServer-master to ESPAsyncWebServer
15. Move the ESPAsyncWebServer folder to your Arduino IDE installation libraries folder

The ESPAsyncWebServer library requires the AsyncTCP library to work. Follow the next steps to install that library:

16. Click here https://github.com/me-no-dev/AsyncTCP/archive/master.zip to download the AsyncTCP library. 
You should have a .zip folder in your Downloads folder
17. Unzip the .zip folder and you should get AsyncTCP-master folder
18. Rename your folder from AsyncTCP-master to AsyncTCP
19. Move the AsyncTCP folder to your Arduino IDE installation libraries folder
20. Finally, re-open your Arduino IDE

------------SPI--with----OLED--------------------------------------------------

SPI	 | MOSI	  |MISO	    |CLK	    |CS
VSPI |GPIO 23	|GPIO 19	|GPIO 18	|GPIO 5
HSPI |GPIO 13	|GPIO 12	|GPIO 14	|GPIO 15

