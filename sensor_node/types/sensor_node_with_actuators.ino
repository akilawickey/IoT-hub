/**
  IoT Sensor Node with Actuators - Nodemcu code
  
  Name: IoT Node
  Purpose: Here we use low power Node MSU esp8266 and following sensors
           AM2301 Humidity and Temperature sensor, BH1750FVI Light sensor, Soil moisture sensor
  @author Akila Wickey
  @version 1.0 05/01/18
  
   Gas sensor BH1750FVI        VCC  –  Wemos 3.3v
                                GND – Wemos Gnd
                                A0 – Wemos A0 
                                
   AM2320 temp humidity sensor  VCC  –  Wemos 3.3v
                                GND – Wemos Gnd
                                SCL – Wemos D3
                                SDA – Wemos D4   
   And you can control any actuator                  
*/
#include <Wire.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <AM2320.h>

#include <Wire.h>

char* lightTopic = "testTopic";

//Actuator Topics
char* topic1_act = "t1";
char* topic2_act = "t2";
char* topic3_act = "t3";
char* topic4_act = "t4";

//Actuator pins
const int act_pin1 = D5;
const int act_pin2 = D6;
const int act_pin3 = D7;
const int act_pin4 = D8;

AM2320 th;

void callback(char* topic, byte* payload, unsigned int length);
//EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "IP_OF_BROKER"

char* tempTopic   = "demo_temp";
char* humTopic   = "demo_hum";
char* gasTopic   = "demo_gas";

char charBuf_temp[50];
char charBuf_hum[50];
char charBuf_gas[50];

// We will take analog input from A0 pin 
const int AnalogIn     = A0; 

WiFiClient wifiClient;

PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void setup() {
  //start the serial line for debugging
  Serial.begin(115200);
  pinMode(act_pin1, OUTPUT);
  pinMode(act_pin2, OUTPUT);
  pinMode(act_pin3, OUTPUT);
  pinMode(act_pin4, OUTPUT);

  
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
      
  Wire.begin(D4,D3);

}

void loop(){

  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  //maintain MQTT connection
  client.loop();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(1000); 

  // Read analog value, in this case a soil moisture

  switch(th.Read()) {
    case 2:
      Serial.println("  CRC failed");
      break;
    case 1:
      Serial.println("  Sensor offline");
      break;
    case 0:
      int data = analogRead(AnalogIn);
      Serial.println(data);
      String humchar = String(th.Humidity);
      String tempchar = String(th.cTemp);
      String gaschar = String(data);
      Serial.println(humchar);
      Serial.println(tempchar);

  
      humchar.toCharArray(charBuf_hum, 50);
      tempchar.toCharArray(charBuf_temp, 50);
      gaschar.toCharArray(charBuf_gas, 50);

      client.publish(humTopic, charBuf_hum);
      client.publish(tempTopic, charBuf_temp);
      client.publish(gasTopic, charBuf_gas);
      
      break;
  }
  
  delay(5000);

}

void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);

   //turn the light on if the payload is '1' and publish to the MQTT server a confirmation message
  if((payload[0] == '1') && (topicStr == "t1")){
    digitalWrite(act_pin1, LOW);
    //client.publish("/test/confirm", "Light On");

  }

  //turn the light off if the payload is '0' and publish to the MQTT server a confirmation message
  if((payload[0] == '0') && (topicStr == "t1")){
    digitalWrite(act_pin1, HIGH);
    //client.publish("/test/confirm", "Light Off");
  }
  
   //turn the light on if the payload is '1' and publish to the MQTT server a confirmation message
  if((payload[0] == '1') && (topicStr == "t2")){
    digitalWrite(act_pin2, LOW);
    //client.publish("/test/confirm", "Light On");

  }

  //turn the light off if the payload is '0' and publish to the MQTT server a confirmation message
  if((payload[0] == '0') && (topicStr == "t2")){
    digitalWrite(act_pin2, HIGH);
    //client.publish("/test/confirm", "Light Off");
  }

     //turn the light on if the payload is '1' and publish to the MQTT server a confirmation message
  if((payload[0] == '1') && (topicStr == "t3")){
    digitalWrite(act_pin3, LOW);
    //client.publish("/test/confirm", "Light On");

  }

  //turn the light off if the payload is '0' and publish to the MQTT server a confirmation message
  if((payload[0] == '0') && (topicStr == "t3")){
    digitalWrite(act_pin3, HIGH);
    //client.publish("/test/confirm", "Light Off");
  }

     //turn the light on if the payload is '1' and publish to the MQTT server a confirmation message
  if((payload[0] == '1') && (topicStr == "t4")){
    digitalWrite(act_pin4, LOW);
    //client.publish("/test/confirm", "Light On");

  }

  //turn the light off if the payload is '0' and publish to the MQTT server a confirmation message
  if((payload[0] == '0') && (topicStr == "t4")){
    digitalWrite(act_pin4, HIGH);
    //client.publish("/test/confirm", "Light Off");
  }

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
        client.subscribe(topic1_act);
        client.subscribe(topic2_act);
        client.subscribe(topic3_act);
        client.subscribe(topic4_act);


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
