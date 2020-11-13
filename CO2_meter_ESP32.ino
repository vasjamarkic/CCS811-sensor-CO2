//#include <dummy.h>
/*Compatible with:
 *    s-Sense CCS811 I2C sensor breakout [PN: SS-CCS811#I2C, SKU: ITBP-6004], info https://itbrainpower.net/sensors/CCS811-CO2-TVOC-I2C-sensor-breakout 
 *    s-Sense CCS811 + HDC2010 I2C sensor breakout [PN: SS-HDC2010+CCS811#I2C, SKU: ITBP-6006], info https://itbrainpower.net/sensors/CCS811-HDC2010-CO2-TVOC-TEMPERATURE-HUMIDITY-I2C-sensor-breakout
 *
 * Reading CO2 and tVOC values example (pulling at 2sec) - based on test software (Beerware license) written by Nathan Seidle from SparkFun Electronics. 
 * 
 * Mandatory wiring:
 *    Common for 3.3V and 5V Arduino boards:           
 *        sensor I2C SDA  <------> Arduino I2C SDA  <-----> ESP32 I2C SDA (D21)
 *        sensor I2C SCL  <------> Arduino I2C SCL  <-----> ESP32 i2C SCL (D22)
 *        sensor GND      <------> Arduino GND      <-----> ESP32 I2C GND
 *        sensor WAKE    <------> Arduino D5 or Arduino GND [connecting to GND the CCS811 sensor will be always ON] <----> ESP32 GND
 *    For Arduino 3.3V compatible:
 *        sensor Vin      <------> Arduino 3.3V
 *    For Arduino 5V compatible:
 *        sensor Vin      <------> Arduino 5V  <------> ESP32 Vin (5V)
 * 
 * Leave other sensor PADS not connected.
 * 
 * SPECIAL note for some ARDUINO boards:
 *        SDA (Serial Data)   ->  A4 on Uno/Pro-Mini, 20 on Mega2560/Due, 2 Leonardo/Pro-Micro
 *        SCK (Serial Clock)  ->  A5 on Uno/Pro-Mini, 21 on Mega2560/Due, 3 Leonardo/Pro-Micro
 * 
 * New CCS811 sensors requires at 48-burn in. Once burned in a sensor requires 20 minutes of run in before readings are considered good.
 * READ CCS811 documentation! https://itbrainpower.net/downloadables/CCS811_DS000459_5-00.pdf
 * 
 * You are legaly entitled to use this SOFTWARE ONLY IN CONJUNCTION WITH s-Sense CCS811 I2C sensors DEVICES USAGE. Modifications, derivates and redistribution 
 * of this software must include unmodified this COPYRIGHT NOTICE. You can redistribute this SOFTWARE and/or modify it under the terms 
 * of this COPYRIGHT NOTICE. Any other usage may be permited only after written notice of Dragos Iosub / R&D Software Solutions srl.
 */

#define SERIAL_SPEED 19200
#include <sSense-CCS811.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>        //synchronise the upload loop
#include <SPI.h>            //for OLED comunication
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <EEPROM.h> 
#include "Plotter.h"

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
int XX;
int YY; // for Arduino listener plotter funcition

CCS811 ssenseCCS811;
AsyncWebServer server(80);
Plotter p;

String readCCS811_CO2() {
  ssenseCCS811.setEnvironmentalData((float)(21.102), (float)(57.73));  // replace with temperature and humidity values from HDC2010 sensor
  if (ssenseCCS811.checkDataAndUpdate())
  {
    DebugPort.print("Measures OK");
    DebugPort.println();
  }
  else if (ssenseCCS811.checkForError())
  {
    ssenseCCS811.printError();
  }
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read CO2 as ppm
  int  CO2 = ssenseCCS811.getCO2();
  if (isnan(CO2)) {    
    Serial.println("Failed to read from CCS881 sensor!");
    return "--";
  }
  else {
    //Serial.println(CO2);
    display.clearDisplay();
    display.display();
    display.setCursor(0,6);  
    display.println("CO2 in ppm:");
    display.setCursor(90,6);
    display.println(CO2);
    display.setCursor(0,18);  
    display.println("tVOC in ppb:");  //blinking issues ... 
    display.display();
    return String(CO2);
    }
  }

String readCCS811_tVOC() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int tVOC= ssenseCCS811.gettVOC();
  if (isnan(tVOC)) {
    Serial.println("Failed to read from CCS881 sensor!");
    return "--";
  }
  else {
    //Serial.println(tVOC);
    display.setCursor(90,18);
    display.println(tVOC);
    display.display();
    return String(tVOC);
  }
 }

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .CCS811-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 CCS811 CO2 Server</h2>
  <p>
    <i class="fas fa-smog" style="color:#00add6;"></i> 
    <span class="CCS811-labels">CO2:</span>
    <span id="CO2">%CO2%</span>
    <sup class="units">ppm</sup>
  </p>
  <p>
    <i class="fas fa-smog" style="color:#a2996d;"></i> 
    <span class="CCS811-labels">tVOC:</span>
    <span id="tVOC">%tVOC%</span>
    <sup class="units">ppb</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("CO2").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/CO2", true);
  xhttp.send();
}, 5000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("tVOC").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/tVOC", true);
  xhttp.send();
}, 5000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with CCS811 values
String processor(const String& var){
  //Serial.println(var);
  if(var == "CO2"){
    return readCCS811_CO2();
  }
  else if(var == "tVOC"){
    return readCCS811_tVOC();
  }
  return String();
}

void setup()
{
  DebugPort.begin(SERIAL_SPEED);
  delay(5000);
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
  display.println("      by Vasko");
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display();
  
  
  DebugPort.println("s-Sense CCS811 I2C sensor.");
  if(!ssenseCCS811.begin(uint8_t(I2C_CCS811_ADDRESS), uint8_t(CCS811_WAKE_PIN), driveMode_1sec))
    DebugPort.println("Initialization failed.");
    
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  display.setCursor(0,8);  
  display.println("     IP address: ");
  display.setCursor(29,16);
  display.println(WiFi.localIP());
  display.display();
  delay(2000);

  p.Begin(); // start plotter
  p.AddTimeGraph( "CO2 [ppm] & tVOC [ppb] plotter", 200000, "CO2", XX, "tVOC", YY );

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/CO2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readCCS811_CO2().c_str());
  });
  server.on("/tVOC", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readCCS811_tVOC().c_str());
  });
  server.begin();

}

void loop()
{ 
  XX = ssenseCCS811.getCO2();
  YY = ssenseCCS811.gettVOC();
  p.Plot();                   //start plotter with typing: ./listener in Xterm! 
   }
