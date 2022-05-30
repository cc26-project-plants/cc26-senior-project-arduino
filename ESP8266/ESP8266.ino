//library
#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN D2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);



// Set WiFi credentials
#define WIFI_SSID "codechrysalis_2.4ghz"
#define WIFI_PASS "foreverbekind"

auto& arduino = Serial1;

void setup() {
  // Setup serial port
  Serial.begin(9600);
//  arduino.begin(9600);
  Serial.println();
  
  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);

   // Connecting to WiFi...
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  // Loop continuously while WiFi is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    waitDelay(200);
  }

  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //DHT INIT
  dht.begin();
  delay(2000);
}

void loop() {
  Serial.println("Waiting for input");
  delay(500);
  waitDelay(100);
  waitDelay(100);
  waitDelay(100);
  delay(1000);

  Serial.println("Checking Serial comm");
  //Try to talk to arduino
  if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte);
  }
}

void waitDelay(int time) {
  delay(time);
  Serial.print(".");
  delay(time);
  Serial.print(".");
  delay(time);
  Serial.print(".");
  delay(time);
  Serial.println();
  }

void printTemp(){
  float tempC = dht.readTemperature();
  if (isnan(tempC)) {
    Serial.println("Failed to read temp from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");
    }
  }

void printHumid(){
 float humid = dht.readHumidity();
 if (isnan(humid)) {
  Serial.println("Failed to read humidity from DHT sensor!");
 }else {
  Serial.print("Humidity: ");
  Serial.print(humid);
  Serial.println("%");
 }
}
