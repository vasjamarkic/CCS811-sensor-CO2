/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  More info how to prog. the charts in HTML: https://api.highcharts.com/highcharts/yAxis.min

for installing in Arduino IDE  the ESP SPIFFS plugin reffer here:  https://www.onetransistor.eu/2019/12/upload-files-esp8266-esp32-spiffs.html 
Copy ESP32FS (find here: https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/tag/1.0) in a folder tools! (create the folder!). Restart Arduino IDE. 
Then upload the data sketch (index.html) via Tools --> ESP32 Sketch Data Upload!
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <Wire.h>
#include <sSense-CCS811.h>
#include <SPI.h>            //for OLED comunication
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <EEPROM.h> 
#include "DHT.h"

#define SERIAL_SPEED 9600
#define DHTPIN 13
#define DHTTYPE DHT11
#define OLED_MOSI 23
#define OLED_CLK 19
#define OLED_DC 18
#define OLED_CS 5
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH 16 

#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const char* ssid = "HUAWEI-B593-F0B1";
const char* password =  "DM9TE62FYEN";
int CO2, tVOC;
float h, t;  //for DHT11

AsyncWebServer server(80);     // Create AsyncWebServer object on port 80
CCS811 ssenseCCS811;
DHT dht(DHTPIN, DHTTYPE);

String readDHTTemperature() {
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature()-3;     //offset needed
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readCCStVOC() {
  float tVOC = ssenseCCS811.gettVOC(); 
  if (isnan(tVOC)) {
    Serial.println("Failed to read from CCS sensor!");
    return "";
  }
  else {
    Serial.println(tVOC);
    return String(tVOC);
  }
}

String readCCSCO2() {
  float CO2 = ssenseCCS811.getCO2();
  if (isnan(CO2)) {
    Serial.println("Failed to read from CCS sensor!");
    return "";
  }
  else {
    Serial.println(CO2);
    return String(CO2);
  }
}
// using function to dispay on OLED
void PUT_data_on_OLED(int CO2, int tVOC, float t, float h) {
  display.setCursor(0,2);  
  display.println("CO2 in ppm:");
  display.setCursor(90,2);
  display.println(CO2);
  display.setCursor(0,12);  
  display.println("tVOC in ppb:");  
  display.setCursor(90,12);
  display.println(tVOC);
  display.setCursor(15,22);   //displ. temp.
  display.println((int)t);
  display.setCursor(30,22);   //displ. C
  display.println(" C");
  display.drawCircle(31,24,2,WHITE);
  display.setCursor(85,22);    //displ. hum.
  display.println((int)h);
  display.setCursor(105,22);    //displ. %
  display.println("%");
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display();
  }

void OLED_Setup() {
  display.setRotation(2);   // rotate the display by 180°
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,8);  
  display.println("      CO2 meter ");
  display.setCursor(0,18);
  display.println("       by STPS");
  display.display();
  delay(1500);
  display.clearDisplay();
  display.display();
  }

void setup(){
  Serial.begin(115200);
  dht.begin();            //start the DHT11 sensor
  delay(3000);
  OLED_Setup();   //init the OLED, rotate, welcome screen
  DebugPort.println("s-Sense CCS811 I2C sensor.");
  if(!ssenseCCS811.begin(uint8_t(I2C_CCS811_ADDRESS), uint8_t(CCS811_WAKE_PIN), driveMode_1sec))
    DebugPort.println("Initialization failed.");
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  //Serial.println("Connected to the WiFi network");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
  display.setCursor(0,8);  
  display.println("     IP address: ");
  display.setCursor(29,16);
  display.println(WiFi.localIP());
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/CO2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readCCSCO2().c_str());
  });
  server.on("/tVOC", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readCCStVOC().c_str());
  });
  // Start server
  server.begin();
}
 
void loop(){
  // Reading temperature or humidity takes about 250 milliseconds!
  h = dht.readHumidity();      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  t = dht.readTemperature()-3;  //Read temperature as Celsius (the default) - offset
  ssenseCCS811.setEnvironmentalData(t, h);  // replace with temperature and humidity values from HDC2010 sensor
  if (ssenseCCS811.checkDataAndUpdate())
  {
    DebugPort.print("Measures OK");
    DebugPort.println();
  }
  else if (ssenseCCS811.checkForError())
  {
    ssenseCCS811.printError();
  }
  CO2 = ssenseCCS811.getCO2(); // Read CO2 as ppm
  tVOC = ssenseCCS811.gettVOC();
  
  PUT_data_on_OLED(CO2, tVOC, t, h);   //screen all the data to OLED display
}
