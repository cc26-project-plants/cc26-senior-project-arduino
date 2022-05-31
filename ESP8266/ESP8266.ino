// Library
#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN D2
#define DHTTYPE DHT22

// Set WiFi credentials
// Code Chrysalis
#define WIFI_SSID "codechrysalis_2.4ghz"
#define WIFI_PASS "foreverbekind"
// Home
// #define WIFI_SSID "ASUS_D0"
// #define WIFI_PASS "FFFFFFFFFF1"

// Constants ********************************************************************************
DHT dht(DHTPIN, DHTTYPE);
float lightData;
float soilWaterData;
float tempC;
float humid;

// Class creation & object creation (THIS MAY BE WRONG!!)************************************
class Plant {
  public:
    int soilWaterLevel;
    int lightLevel;
    float humidLevel;
    float temp;

  // Methods
  void updateSoilWaterLevel(int value) {
    soilWaterLevel = value;
  }
};

Plant fakePlant;
fakePlant.soilWaterLevel = 1;
fakePlant.lightLevel = 20;
fakePlant.humidLevel = 3.0;
fakePlant.temp = 10.0;

// Setup function ***************************************************************************
void setup() {
  Serial.begin(9600);
  Serial.println();

  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  setUpWiFi();
  Serial.println();

  // DHT init
  dht.begin();
  delay(2000);
}

// Main program *****************************************************************************
void loop() {
  Serial.print("Fake plant soil water level: ");
  Serial.println(fakePlant.soilWaterLevel);
  delay(2000);
  Serial.println("Updating soil water level...");
  fakePlant.updateSoilWaterLevel(500);
  delay(1000);
  Serial.print("Fake plant soil water level is now: ");
  Serial.println(fakePlant.soilWaterLevel);
  delay(2000);

  Serial.println(analogRead(A0));
  readTemp();
  delay(500);
}

// Functions ********************************************************************************
void setUpWiFi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    waitDelay(200);
  }

  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
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

void updateLight() {
  // 
}

void readTemp() {
  tempC = dht.readTemperature();
  if (isnan(tempC)) {
    Serial.println("Failed to read temp from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");
  }
}

void readHumid() {
  humid = dht.readHumidity();
  if (isnan(humid)) {
    Serial.println("Failed to read humidity from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humid);
    Serial.println("%");
  }
}
