#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Code Chrysalis
#define WIFI_SSID "codechrysalis_2.4ghz"
#define WIFI_PASS "foreverbekind"
// Home
// #define WIFI_SSID "ASUS_D0"
// #define WIFI_PASS "FFFFFFFFFF1"

#define DHTPIN D2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

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
    Serial.println("Light Level: " + String(lightLevel));
    Serial.println("Soil Water Level: " + String(soilWaterLevel));
    Serial.println("Temperature: " + String(temperature) + "~C");
    Serial.println("Humidity: " + String(temperature) + "%");
  }
};

// Declare instances/variables *************************************************
WiFiClient client;
HTTPClient http;
Plant fakePlant;

String URL = "http://happa-26-backend.an.r.appspot.com/plantStats/";
String PLANT_ID = "VAlAa3aEtub3qSw7SIjz";

// Setup function ****************************************************
void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  setUpWiFi();

  dht.begin();
  delay(2000);
}

// Main program ******************************************************
void loop() {
  fakePlant.updateAll();
  fakePlant.printAll();
  waitDelay(3000);

  postRequest();
  
  // Delay for 5 minutes
  delay(300000);
  
  // Delay for 1 hour
  // delay(3600000);
}

// Functions *********************************************************
void setUpWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    waitDelay(200);
  }
  Serial.print("Wifi connected. IP address: ");
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

void postRequest() {
  Serial.println("Sending post request...");
  
  String jString = "{\"soilWaterLevel\":" + String(fakePlant.soilWaterLevel)
                   + ",\"lightLevel\":" + String(fakePlant.lightLevel)
                   + ",\"humidityLevel\":" + String(fakePlant.humidityLevel)
                   + ",\"temperature\":" + String(fakePlant.temperature)
                   + "}";
  Serial.println("JSON string to be sent : " + jString);
  
  http.begin(client, URL + PLANT_ID);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jString);
  if (httpCode > 0) {
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      const String& payload = http.getString();
      Serial.println("received payload:\n<<" + String(payload) + ">>\n");
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
