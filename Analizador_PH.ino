#include "Arduino.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

int threshold = 40;
bool touchActive = false;
bool lastTouchActive = false;
bool testingLower = true;

WiFiMulti WiFiMulti;
HTTPClient ask;

const char* ssid     = "galaxy_pau"; //Wifi SSID
const char* password = "wifipau99"; //Wifi Password
const char* apiKeyIn = "xuy9v3TkMkNEpenZj3ZdQjfVCVnZPt9A";      // API KEY IN
const unsigned int writeInterval = 1000;

// ASKSENSORS API host config
const char* host = "api.asksensors.com";  // API host name
const int httpPort = 80;      // port

void gotTouchEvent(){
  if (lastTouchActive != testingLower) {
    touchActive = !touchActive;
    testingLower = !testingLower;
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(1000); 
  Serial.println("ESP32 Touch Interrupt Test");
  touchAttachInterrupt(T2, gotTouchEvent, threshold);

  Serial.println("********** Program Start : Connect ESP32 to AskSensors.");
  Serial.println("Wait for WiFi... ");

  // connecting to the WiFi network
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  // connected
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

 if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }else {
     String url = "http://api.asksensors.com/write/";
    url += apiKeyIn;
    url += "?module1=";

    #ifdef RGB_BUILTIN
    if (touchRead(T2) < 10000) {
      neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,RGB_BRIGHTNESS,0);
       url += random(2, 5);
      delay(125);
    } else if (touchRead(T2) < 20000) {
      neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0);
      url += random(5, 8);
      delay(125);
    }else{
      neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,0,0);
      url += random(8, 10);
      delay(125);
      }
    Serial.printf("T2 pin2 = %d \n", touchRead(T2));
    delay(125);
  #endif
    
  Serial.print("********** requesting URL: ");
  Serial.println(url);
  ask.begin(url); //Specify the URL
  int httpCode = ask.GET();          
 
  if (httpCode > 0) { 
    String payload = ask.getString();
    Serial.println(httpCode);
    Serial.println(payload);
  } else {
    Serial.println("Error on HTTP request");
  }
  ask.end(); //End 
  Serial.println("********** End ");
  Serial.println("*****************************************************");

}
  client.stop();
  delay(writeInterval);
}
