#include <MQTTClient.h>
#include <SPI.h>
#include <WiFi.h>
#include <Bridge.h>
#include "DHT.h"
#include <LiquidCrystal.h>
#define DHTPIN 8     // what digital pin we're connected to
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);
// pin of the button connected to the arduino board
const int buttonPin = 1;
// pin of the led light
// it's actually also the default arduino debug light
const int ledPin = 13;
const int soil = A5;
const int light = A4;
int soil_value = 0;
int light_value = 0;

// simple variables used to store the led and the button statuses
String buttonStatus = "off";
String ledStatus = "off";

// it will hold all the messages coming from the nodejs server
String inputString = "";

// whether the string received form nodejs is complete
// a string is considered complete by the carriage return '\r'
boolean stringComplete = false;

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
//wifi settings
char ssid[] = "AirDroidAP";          //  your network SSID (name)
char pass[] = "123456789";      // your network password
int status = WL_IDLE_STATUS;   // the Wifi radio's status

//mqtt connection settings
String entityID = "";
String mqttServer = "192.168.43.222";
int mqttServerPort = 1883;
//String pubTopic = "iot/" + entityID + "/pub";
//String subTopic = "iot/" + entityID + "/sub";

String pubTopic = "hello/world";
String subTopic = "hello/world";  




WiFiClient net;
MQTTClient client(mqttServer.c_str(),1883, net);


void setup() {
  Serial.begin(9600);
 
  pinMode(buttonPin, INPUT);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  pinMode(9, OUTPUT);

  dht.begin();
  lcd.begin(16, 4);
     // set up the LCD's number of columns and rows:
 
  while ( status != WL_CONNECTED) {
    Serial.println();
    Serial.print("Connecting by WPA to SSID: " + (String)ssid + " ...");
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("connected.");
  while (!client.connected()) {
    Serial.print("Connecting to mosquitto server: ");
    Serial.print(mqttServer);
    Serial.print(":");
    Serial.println(mqttServerPort);
    client.connect(entityID.c_str());
  }
  
  Serial.println("Connection with mqtt server established.");
  Serial.print("subscribing to topic: " + subTopic + " ...");
  client.subscribe(subTopic);
  Serial.println("subscribed.");
  Serial.println();
  Serial.println("Publish to topic: " + pubTopic);
  Serial.println();

   
}


void loop() {

  
   
  
  if(client.connected()) {
//    the data will be insert with measurement server timestamp
//  Any data row that will be publish can not contain more then 80 letters.


//////////////////////////////////////////////////////////////////////////////////////

    soil_value= analogRead(soil);
    light_value= analogRead(light);
    //soil_value= soil_value/10;
   Serial.println(soil_value);
   Serial.println();
   Serial.println(light_value);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);


  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  
//
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

    // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  lcd.print("humidity");
  lcd.setCursor(10, 0);
  // print the number of seconds since reset:
  lcd.print(h);
  lcd.setCursor(0, 1);
  lcd.print("Temper");
   lcd.setCursor(10, 1);
  // print the number of seconds since reset:
  lcd.print(t);
  lcd.setCursor(15, 1);
  // print the number of seconds since reset:
  lcd.print("C");
  lcd.setCursor(10, 2);
  // print the number of seconds since reset:
  lcd.print(f);
  lcd.setCursor(0, 2);
   lcd.print("soil mois");
  lcd.setCursor(15, 2);
  // print the number of seconds since reset:
  lcd.print(soil_value);
   lcd.setCursor(0, 3);
   lcd.print("light inten");
  lcd.setCursor(15, 3);
  // print the number of seconds since reset:
  lcd.print(light_value);


    String data = "series e:" + entityID + " m:millis=" + (String)getData();
    Serial.println("sending row: '" + data + "' ...");
    client.publish(pubTopic,data);
    Serial.println("sended.");
    String temperature = "series e:" + entityID + " m:temperature=" + (String)getTemperature();  
    Serial.println("sending row: '" + temperature + "' ...");
    client.publish(pubTopic,temperature);
    Serial.println("sended.");
    String humidity = "series e:" + entityID + " m:humidity=" + (String)getHumidity();  
    Serial.println("sending row: '" + humidity + "' ...");
    client.publish(pubTopic,humidity);
    String soil = "series e:" + entityID + " m:soil moisture=" + (String)soil_value;  
    Serial.println("sending row: '" + soil + "' ...");
    client.publish(pubTopic,soil);
    Serial.println("sended.");
    client.loop();
    delay(1000);


//////////////////////////////////////////////////////////////////////////////////////////
    
  } else {
    Serial.println("Disconnected from server. Require to restart device.");
    while(true){delay(5000);}
  }

}


double getData() {
  return 200.0*sin((double)millis()/100000.0) + 300.0;
}

float getTemperature() {
  return dht.readTemperature();
}
float getHumidity() {
  return dht.readHumidity();
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  Serial.print("incomming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
}
