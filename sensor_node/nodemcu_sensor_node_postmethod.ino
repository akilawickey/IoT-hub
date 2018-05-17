
/*
IoT Greenhouse Code

Akila Wickey 

This IoT Greenhouse consists of low power Node MSU esp8266 and few sensors
Here i have used AM2301 Humidity and Temperature sensor, BH1750FVI Light sensor, Soil moisture sensor and Rain drop detection sensor    

VCC  –  Wemos 3.3v
GND – Wemos Gnd
SCL – Wemos D1
SDA – Wemos D2
*/

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <Wire.h>
#include <BH1750.h>
#include "DHT.h"

#define DHTPIN D4 
#define LIGHT_sensor D3

#define DHTTYPE DHT21   // DHT 21 (AM2301)
BH1750 lightMeter;

const char* ssid = "SSID";
const char* password = "PASSWORD";
// The ID below comes from Google Sheets.
// Towards the bottom of this page, it will explain how this can be obtained
const char *GScriptId = "Your ID ";
// web app url https://script.google.com/macros/s/AKfycby42E0NLno0D6L66odVyMFm9YTBw2LhWE0uyabWRgPpNE3PhJSj/exec
// Push data on this interval
const int dataPostDelay = 30000;  // 15 minutes = 15 * 60 * 1000

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";

const int httpsPort =  443;
HTTPSRedirect client(httpsPort);

// Prepare the url (without the varying data)
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";

// We will take analog input from A0 pin 
const int AnalogIn     = A0; 
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  lightMeter.begin();
  Serial.println("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print(String("Connecting to "));
  Serial.println(host);

  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  // Connection Status, 1 = Connected, 0 is not.
  Serial.println("Connection Status: " + String(client.connected()));
  Serial.flush();
  
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    Serial.flush();
    return;
  }

  // Data will still be pushed even certification don't match.
  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }
}

// This is the main method where data gets pushed to the Google sheet
void postData(String tag, float value){
  if (!client.connected()){
    Serial.println("Connecting to client again..."); 
    client.connect(host, httpsPort);
  }
  String urlFinal = url + "tag=" + tag + "&value=" + String(value);
  client.printRedir(urlFinal, host, googleRedirHost);
}

// Continue pushing data at a given interval
void loop() {
 // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Read analog value, in this case a soil moisture
  int data = analogRead(AnalogIn);

  // get the light sensor values
  uint16_t lux = lightMeter.readLightLevel();
 
  // Post these information
  postData("SoilMoisture", data);
  postData("Humidity",h);
  postData("Temperature",t);
  postData("Light",lux);
  delay (dataPostDelay);
}
