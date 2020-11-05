//#include <dummy.h>
/*Compatible with:
 *    s-Sense CCS811 I2C sensor breakout [PN: SS-CCS811#I2C, SKU: ITBP-6004], info https://itbrainpower.net/sensors/CCS811-CO2-TVOC-I2C-sensor-breakout 
 *    s-Sense CCS811 + HDC2010 I2C sensor breakout [PN: SS-HDC2010+CCS811#I2C, SKU: ITBP-6006], info https://itbrainpower.net/sensors/CCS811-HDC2010-CO2-TVOC-TEMPERATURE-HUMIDITY-I2C-sensor-breakout
 *
 * Reading CO2 and tVOC values example (pulling at 2sec) - based on test software (Beerware license) written by Nathan Seidle from SparkFun Electronics. 
 * Thank you Nathan! Great job! 
 * 
 * We've ported Nathan's functions into a class, add some variables, functions and fuctionalities.
 * 
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
 * WIRING WARNING: wrong wiring may damage your Arduino board MCU or your sensor! Double check what you've done.
 * 
 * New CCS811 sensors requires at 48-burn in. Once burned in a sensor requires 20 minutes of run in before readings are considered good.
 * READ CCS811 documentation! https://itbrainpower.net/downloadables/CCS811_DS000459_5-00.pdf
 * 
 * You are legaly entitled to use this SOFTWARE ONLY IN CONJUNCTION WITH s-Sense CCS811 I2C sensors DEVICES USAGE. Modifications, derivates and redistribution 
 * of this software must include unmodified this COPYRIGHT NOTICE. You can redistribute this SOFTWARE and/or modify it under the terms 
 * of this COPYRIGHT NOTICE. Any other usage may be permited only after written notice of Dragos Iosub / R&D Software Solutions srl.
 * 
 * This SOFTWARE is distributed is provide "AS IS" in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *  
 * Dragos Iosub, Bucharest 2019.
 * https://itbrainpower.net
 */

#define SERIAL_SPEED 19200
#include <sSense-CCS811.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>        //synchronise the upload loop
//#include <SPI.h>

const char* ssid = "HUAWEI-B593-F0B1";
const char* password =  "DM9TE62FYEN";

CCS811 ssenseCCS811;
AsyncWebServer server(80);

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
    Serial.println(CO2);
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
    Serial.println(tVOC);
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
    <i class="fas fa-head-side-mask" style="color:#00add6;"></i> 
    <span class="CCS811-labels">CO2:</span>
    <span id="CO2">%CO2%</span>
    <sup class="units">ppm</sup>
  </p>
  <p>
    <i class="fas fa-smog" style="color:#00add6;"></i> 
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
  //Serial.begin(115200);
  DebugPort.begin(SERIAL_SPEED);
  delay(5000);
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

   }
