/*
 *  HTTPS with follow-redirect example
 *  Created by Sujay S. Phadke, 2016
 *  
 *  Based on the WifiClientSecure example by
 *  Ivan Grokhotkov
 *  *
 *  This example is in public domain.
 */

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DHT.h"

#define DHTPIN D0  

// Replace with your network credentials
const char* ssid = "";
const char* password = "";

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";
const char *GScriptId = "AKfycbxIyRVLnWZVv4yog8JUkQcyk7jByufQTM5wVmFtreKjrBjGbXI";


const int httpsPort = 443;

// http://askubuntu.com/questions/156620/how-to-verify-the-ssl-fingerprint-by-command-line-wget-curl/
// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
// www.grc.com doesn't seem to get the right fingerprint
// SHA1 fingerprint of the certificate
//const char* fingerprint = "94 2F 19 F7 A8 B4 5B 09 90 34 36 B2 2A C4 7F 17 06 AC 6A 2E";
const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";
const char* fingerprint2 = "94 64 D8 75 DE 5D 3A E6 3B A7 B6 15 52 72 CC 51 7A BA 2B BE";

void setup()
{
  Serial.begin(115200);
//  DHTPIN.begin();
  Serial.print("Connecting to " + *ssid);
  WiFi.begin(ssid, password);
  Serial.println("going into wl connect");
  while (WiFi.status() != WL_CONNECTED) //not connected,..waiting to connect
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("wl connected");
  Serial.println("");
  Serial.println("Credentials accepted! Connected to wifi\n ");
  Serial.println("");
}
void loop()
{
  float Humidity = 21.3;
  float Temperature = 11.2;
  
//  float Humidity = DHT.humidity;
//  float Temperature = DHT.temperature;
  String url = String("/macros/s/") + GScriptId + String("/exec?Temperature=") + Temperature + String("&Humidity=") + Humidity;
  if (isnan(Humidity) || isnan(Temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(Humidity);
  Serial.print(" %\t");
  Serial.print("Temperature in Cel: ");
  Serial.print(Temperature);
  Serial.print(" *C ");
  HTTPSRedirect client(httpsPort);
  Serial.print("Connecting to ");
  Serial.println(host);
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  Serial.flush();
  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  Serial.flush();
  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }
  client.printRedir(url, host, googleRedirHost);
  Serial.println("==============================================================================");
  {
    HTTPSRedirect client(httpsPort);
  }
  if (!client.connected())
    client.connect(host, httpsPort);
  Serial.println("closing connection");
  delay(1000);                       // Send data every 1 hour
}
