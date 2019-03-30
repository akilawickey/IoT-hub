/**
  IoT Sensor Node for any Sensor - Nodemcu code
  
  Name: my_multiply_doc
  Purpose: Here we use low power Node MSU esp8266 and following sensors
           AM2301 Humidity and Temperature sensor, BH1750FVI Light sensor, Soil moisture sensor
  @author Akila Wickey
  @version 1.0 05/01/18

   Light sensor BH1750FVI       VCC  –  Wemos 3.3v
                                GND – Wemos Gnd
                                SCL – Wemos D1
                                SDA – Wemos D2
                                
   Soil sensor BH1750FVI        VCC  –  Wemos 3.3v
                                GND – Wemos Gnd
                                A0 – Wemos A0 
                                
   AM2301 temp humidity sensor  VCC  –  Wemos 3.3v
                                GND – Wemos Gnd
                                Digital pin  – Wemos D5 
                     
*/
#include <Wire.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <Wire.h>
#include <BH1750.h>

#include "DHTesp.h"
#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif
DHTesp dht;
//#define LIGHT_sensor D3
BH1750 lightMeter;

void callback(char* topic, byte* payload, unsigned int length);
//EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "MQ IP"

const int lightPin = D5;

char* lightTopic   = "light";
char* tempTopic   = "temp";
char* humTopic   = "hum";
char* soilTopic   = "soil";

char charBuf_temp[50];
char charBuf_hum[50];
char charBuf_light[50];
char charBuf_soil[50];

// We will take analog input from A0 pin 
const int AnalogIn     = A0; 

WiFiClient wifiClient;

PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void setup() {
  //initialize the light as an output and set to LOW (off)
  pinMode(lightPin, OUTPUT);
  digitalWrite(lightPin, HIGH);

  //start the serial line for debugging
  Serial.begin(115200);
  
  WiFiManager wifiManager;
  wifiManager.setTimeout(180);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  wifiManager.setAPCallback(configModeCallback);

  Serial.println("connected...yeey :)");

  delay(100);
  
   if(!wifiManager.autoConnect()) {
      Serial.println("failed to connect and hit timeout");
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(1000);
    } 

  //wait a bit before starting the main loop
  delay(5000);
  dht.setup(D5, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
      
  Wire.begin(); //light sensor SCL and SDA
  lightMeter.begin();

}

void loop(){

  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  //maintain MQTT connection
  client.loop();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(1000); 
  delay(dht.getMinimumSamplingPeriod());

  float h = dht.getHumidity();
  Serial.println(h);
  // Read temperature as Celsius (the default)           
  float t = dht.getTemperature();
  Serial.println(t);
  // Read analog value, in this case a soil moisture
  int data = analogRead(AnalogIn);
  Serial.println(t);

  // get the light sensor values
  uint16_t lux = lightMeter.readLightLevel();
  Serial.println(lux);

  String humchar = String(h);
  String tempchar = String(t);
  String soilchar = String(data);
  String lightchar = String(lux);
  
  humchar.toCharArray(charBuf_hum, 50);
  tempchar.toCharArray(charBuf_temp, 50);
  soilchar.toCharArray(charBuf_soil, 50);
  lightchar.toCharArray(charBuf_light, 50);
  
//  Serial.println(charBuf_temp+ ' '+tempchar+' '+soilchar+ ' ' + lightchar);
//  client.publish(nodeTopic, "node/badulla/1/test");
  client.publish(tempTopic, charBuf_temp);
  client.publish(humTopic, charBuf_hum);
  client.publish(soilTopic, charBuf_soil);
  client.publish(lightTopic, charBuf_light);
  
  delay(5000);

}

void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);

}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void reconnect() {
  
  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED){
    //debug printing
    Serial.print("Connecting to ");
//    Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    //print out some more debug once connected
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        Serial.print("\tMTQQ Connected");
        client.subscribe(lightTopic);

      }

      //otherwise print failed for debugging
      else{Serial.println("\tFailed."); abort();}
    }
  }
}

//generate unique name from MAC addr
String macToStr(const uint8_t* mac){

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5){
      result += ':';
    }
  }

  return result;
}
