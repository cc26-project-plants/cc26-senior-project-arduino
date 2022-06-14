#include <DHT.h>
#include <Adafruit_ADS1X15.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <millisDelay.h>
#include <ArduinoJson.h>
#include "credentials.h"

// Fixed values
#define DHT_PIN D3
#define PUMP_POWER_PIN D5
#define LIGHT_POWER_PIN D6
#define LED_PIN D7
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
Adafruit_ADS1115 analogChip;

const int printStatusInterval = 1000 * 60;
const int postStatusInterval = 1000 * 60 * 60;
const int mqttConnectionInterval = 1000 * 5;
const int mqttPublishInterval = 1000 * 60 * 10;

// Plant class
class Plant {
public:
  int lightLevel = 0;
  int soilWaterLevel = 0;
  float temperature = 0.0;
  float humidityLevel = 0.0;

  void updateLightLevel() {
    lightLevel = analogChip.readADC_SingleEnded(0);
  }

  void updateSoilWaterLevel() {
    soilWaterLevel = analogChip.readADC_SingleEnded(1);
  }

  void updateTemperature() {
    temperature = dht.readTemperature();
  }

  void updateHumidityLevel() {
    humidityLevel = dht.readHumidity();
  }

  void writeBrightness(String input) {
    int brightness = input == "1" ? 255 : 0;

    if (brightness) {
      digitalWrite(LIGHT_POWER_PIN, HIGH);
    } else {
      digitalWrite(LIGHT_POWER_PIN, LOW);
    }
    
    analogWrite(LED_PIN, brightness);
    Serial.println("[sensor] Brightness: " + String(brightness));
  }

  void water() {
    digitalWrite(PUMP_POWER_PIN, HIGH);
    delay(2000);
    digitalWrite(PUMP_POWER_PIN, LOW);

    Serial.println("[sensor] Finished watering plant");
  }

  void updateAll() {
    updateSoilWaterLevel();
    updateLightLevel();
    updateHumidityLevel();
    updateTemperature();
  }

  void printAll() {
    Serial.println("[sensor] Light Level: " + String(lightLevel));
    Serial.println("[sensor] Soil Water Level: " + String(soilWaterLevel));
    Serial.println("[sensor] Temperature: " + String(temperature) + "~C");
    Serial.println("[sensor] Humidity: " + String(humidityLevel) + "%");
  }
};

// Instances
Plant happa;
WiFiClient wifiClient;
HTTPClient http;
PubSubClient mqttClient(wifiClient);

millisDelay printStatusDelay;
millisDelay postStatusDelay;
millisDelay mqttConnectionDelay;
millisDelay mqttPublishDelay;

// Main
void setup() {
  Serial.begin(115200);
  Serial.println("------------ SET UP START ------------");  

  setupDelays();
  setupSensors();
  setUpWiFi();
  setupHTTP();
  setupMQTT();
  subscribeTopics();

  Serial.println("------------- SET UP END -------------");
}

void loop() {
  mqttClient.loop();
  
  checkPrintStatusDelay();
  checkPostStatusDelay();
  checkMqttConnectionDelay();
  checkMqttPublishDelay();
}

// Functions
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

// Set up
void setupDelays() {
  printStatusDelay.start(1000);
  postStatusDelay.start(1000);
  mqttConnectionDelay.start(100);
  mqttPublishDelay.start(100);
}

void setupSensors() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(LIGHT_POWER_PIN, OUTPUT);
  pinMode(PUMP_POWER_PIN, OUTPUT);
  digitalWrite(LIGHT_POWER_PIN, LOW);
  digitalWrite(PUMP_POWER_PIN, LOW);
  dht.begin();
  setupAnalogChip();
}

void setupAnalogChip() {
  if (!analogChip.begin()) {
    Serial.println("[sensor] Failed to initialize analogChip.");
    while (1);
  }
}

void setUpWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    waitDelay(200);
  }
  Serial.print("[Wifi] IP address: ");
  Serial.println(WiFi.localIP());
}

void setupHTTP() {
  http.begin(wifiClient, String(URL) + String(PLANT_ID));
  http.addHeader("Content-Type", "application/json");
}

void setupMQTT() {
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);

  Serial.println("[MQTT] connecting MQTT...");
  if (!mqttClient.connect(PLANT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    reconnectMQTT();
  }

  Serial.println("[MQTT] connected to MQTT with device: " + String(PLANT_ID));
  subscribeTopics();
}

// Check delays
void checkPrintStatusDelay() {
  if (!printStatusDelay.justFinished()) return;
  Serial.println("--- delay --- print plant status");

  printPlantStatus();
  printStatusDelay.start(printStatusInterval);
  Serial.println("");
}

void checkPostStatusDelay() {
  if (!postStatusDelay.justFinished()) return;
  Serial.println("--- delay --- post plant status");

  postPlantStatus();
  postStatusDelay.start(postStatusInterval);
  Serial.println("");
}

void checkMqttConnectionDelay() {
  if (!mqttConnectionDelay.justFinished()) return;
  Serial.println("--- delay --- checking connection");

  if (!mqttClient.connect(PLANT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    reconnectMQTT();
  }
  mqttConnectionDelay.start(mqttConnectionInterval);
  Serial.println("");
}

void checkMqttPublishDelay() {
  if (!mqttPublishDelay.justFinished()) return;
  Serial.println("--- delay --- publish topic");

  publishMessage(TOPIC_DEVICE, PLANT_ID);
  mqttPublishDelay.start(mqttPublishInterval);
  Serial.println("");
}

// Sensors
void printPlantStatus() {
  happa.updateAll();
  happa.printAll();
}

// HTTP
void postPlantStatus() {
  int httpCode = sendPostRequest();

  if (httpCode) {
    Serial.printf("[HTTP] POST status code: %d\n", httpCode);
    if (httpCode >= 200 && httpCode < 300) {
      const String& payload = http.getString();
      Serial.println("[HTTP] received payload:\n<<" + payload + ">>\n");
    }
  } else {
    Serial.printf("[HTTP] POST error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

int sendPostRequest() {
  String sensorData = buildJSON();
  int httpCode = http.POST(sensorData);
  return httpCode;
}

String buildJSON() {
  String sensorData = "{\"soilWaterLevel\":" + String(happa.soilWaterLevel)
                      + ",\"lightLevel\":" + String(happa.lightLevel)
                      + ",\"humidityLevel\":" + String(happa.humidityLevel)
                      + ",\"temperature\":" + String(happa.temperature)
                      + "}";
  Serial.println("[HTTP] JSON to be sent : " + sensorData);
  return sensorData;
}

// MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("____");
  Serial.println("[MQTT] message received from topic: " + String(topic));

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);
  checkTopics(topic, message);
}

void checkTopics(char* topic, String message) {
  if (topic == TOPIC_LIGHT_req) {
    happa.writeBrightness(message);
    if (message == "1") {
      publishMessage(TOPIC_LIGHT_res, "on");
    } else {
      publishMessage(TOPIC_LIGHT_res, "off");
    }
  } else if (topic == TOPIC_WATER_req) {
    happa.water();
    publishMessage(TOPIC_WATER_res, "done");
  }
}

void reconnectMQTT() {
  while (!mqttClient.connect(PLANT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    mqttClient.state();
    Serial.println("[MQTT] connection failed");
    Serial.println("[MQTT] reconnecting");
    waitDelay(200);
  }
}

void subscribeTopics() {
  int status_light = mqttClient.subscribe(TOPIC_LIGHT_req);
  // int status_water = mqttClient.subscribe(TOPIC_WATER_req);
  
  if (status_light) {
    Serial.println("[MQTT] subscribed to: " + String(TOPIC_LIGHT_req));
    // Serial.println("[MQTT] subscribed to: " + String(TOPIC_WATER_req));
  } else {
    Serial.println("[MQTT] subscribe failed");
    reconnectMQTT();
    subscribeTopics();
  }
}

void publishMessage(const char* topic, const char* message) {
  boolean status = mqttClient.publish(topic, message);
    
  if (status) {
    Serial.println("[MQTT] published topic: " + String(topic));
  } else {
    Serial.println("[MQTT] publish failed");
    reconnectMQTT();
  }

  waitDelay(200);
}
