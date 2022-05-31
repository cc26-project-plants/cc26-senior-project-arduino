// Library
#include <ESP8266WiFi.h>
#include "DHT.h"

// Set temperature humidity sensor
#define DHTPIN D2
#define DHTTYPE DHT22

// Set WiFi credentials
// Code Chrysalis
#define WIFI_SSID "codechrysalis_2.4ghz"
#define WIFI_PASS "foreverbekind"
// Home
// #define WIFI_SSID "ASUS_D0"
// #define WIFI_PASS "FFFFFFFFFF1"

// Class *************************************************************
class Plant;
class Plant {
public:
  int lightLevel = 0;
  int soilWaterLevel = 0;
  float temperature = 0.0;
  float humidityLevel = 0.0;

  // Light Sensor *************************
  void updateLightLevel() {
    lightLevel = analogRead(A0);
  }
  int getLightLevel() {
    return lightLevel;  
  }

  // Soil Water Sensor ********************
  void updateSoilWaterLevel(int value) {
    soilWaterLevel = value;
  }
  int getSoilWaterLevel() {
    return soilWaterLevel;  
  }

  // Temperature Sensor *******************
  void updateTemperature() {
    temperature = dht.readTemperature();
  }
  int getTemperature() {
    return temperature;  
  }

  // Humidity Sensor **********************
  void updateHumidityLevel() {
    humidityLevel = dht.readHumidity();
  }
  int getHumidityLevel() {
    return humidityLevel;  
  }

  // General methods **********************
  void updateAll() {
    updateSoilWaterLevel(333);
    updateLightLevel();
    updateHumidityLevel();
    updateTemperature();
  }

  void printAll() {
    Serial.println("Light Level: " + lightLevel);
    Serial.println("Soil Water Level: " + soilWaterLevel);
    Serial.println("Temperature: "+ temperature + "~C");
    Serial.println("Humidity: " + temperature + "%");
  }
};

// Constants *********************************************************
DHT dht(DHTPIN, DHTTYPE);
Plant fakePlant;

// Setup function ****************************************************
void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  setUpWiFi();

  dht.begin();
  delay(2000);
}

// Main program ******************************************************
void loop() {
  fakePlant.updateAll();
  fakePlant.printAll();
  waitDelay(1000);

  Serial.println("TEST");
}

// Functions *********************************************************
void setUpWiFi() {
  Serial.println("Connecting to " + WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    waitDelay(200);
  }
  Serial.println("Connected! IP address: " + WiFi.localIP());
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
