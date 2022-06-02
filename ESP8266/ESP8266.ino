#include <string>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Fixed values ******************************************************
#define LIGHTPIN A0
#define DHTPIN D2
#define DHTTYPE DHT22

// Code Chrysalis
#define WIFI_SSID "codechrysalis_2.4ghz"
#define WIFI_PASS "foreverbekind"
// Home
// #define WIFI_SSID "ASUS_D0"
// #define WIFI_PASS "FFFFFFFFFF1"

#define URL = "http://happa-26-backend.an.r.appspot.com/plantStats/"
#define PLANT_ID = "VAlAa3aEtub3qSw7SIjz"  // Thomas: "wdNtSRStxaQU9gc2QWM7"

// Class *************************************************************
class Plant {
public:
  DHT dht(DHTPIN, DHTTYPE);
  int lightLevel = 0;
  int soilWaterLevel = 0;
  float temperature = 0.0;
  float humidityLevel = 0.0;

  // Update values
  void updateLightLevel() {
    lightLevel = analogRead(LIGHTPIN);
  }

  void updateSoilWaterLevel(int value) {
    soilWaterLevel = value;
  }

  void updateTemperature() {
    temperature = dht.readTemperature();
  }

  void updateHumidityLevel() {
    humidityLevel = dht.readHumidity();
  }

  // Read values
  int getLightLevel() {
    return lightLevel;  
  }

  int getSoilWaterLevel() {
    return soilWaterLevel;  
  }

  int getTemperature() {
    return temperature;  
  }

  int getHumidityLevel() {
    return humidityLevel;  
  }

  void updateAll() {
    updateSoilWaterLevel(333);
    updateLightLevel();
    updateHumidityLevel();
    updateTemperature();
  }

  void printAll() {
    Serial.println("Light Level: " + String(lightLevel));
    Serial.println("Soil Water Level: " + String(soilWaterLevel));
    Serial.println("Temperature: " + String(temperature) + "~C");
    Serial.println("Humidity: " + String(temperature) + "%");
  }
};

// Main **************************************************************
void setup() {
  Plant plantTest;
  HTTPClient http;

  Serial.begin(115200);
  dht.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  setUpWiFi();
  setupHTTP();
}

void loop() {
  plantTest.updateAll();
  plantTest.printAll();
  waitDelay(3000);

  postRequest();
  
  // Delay for 5 minutes
  delay(300000);
  
  // Delay for 1 hour
  // delay(3600000);
}

// Functions *********************************************************
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

void setUpWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    waitDelay(200);
  }
  Serial.print("[Wifi] IP address: ");
  Serial.println(WiFi.localIP());
}

void setupHTTP() {
  http.begin(client, URL + PLANT_ID);
  http.addHeader("Content-Type", "application/json");
}

void postRequest() {
  int httpCode = sendPostRequest();

  if (httpCode) {
    Serial.printf("[HTTP] POST status code: %d\n", httpCode);
    if (httpCode => 200 && httpCode < 300) {
      const String& payload = http.getString();
      Serial.println("received payload:\n<<" + payload + ">>\n");
    }
  } else {
    Serial.printf("[HTTP] POST error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

string buildJSON() {
  string sensorData = "{\"soilWaterLevel\":" + String(plantTest.soilWaterLevel)
                      + ",\"lightLevel\":" + String(plantTest.lightLevel)
                      + ",\"humidityLevel\":" + String(plantTest.humidityLevel)
                      + ",\"temperature\":" + String(plantTest.temperature)
                      + "}";
  Serial.println("JSON to be sent : " + sensorData);
  return sensorData;
}

int sendPostRequest() {
  string sensorData = buildJSON();
  int httpCode = http.POST(sensorData);
  return httpCode;
}
