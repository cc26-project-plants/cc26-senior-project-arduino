#include <DHT.h>
#include <Adafruit_ADS1X15.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ****************************************************** Fixed values
// Sensors
#define DHT_PIN D3
#define PUMP_POWER_PIN D5
#define LIGHT_POWER_PIN D6
#define LED_PIN D7
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
Adafruit_ADS1115 analogChip;

// WiFi
// Code Chrysalis
#define WIFI_SSID "codechrysalis_2.4ghz"
#define WIFI_PASS "foreverbekind"
// Home
// #define WIFI_SSID "ASUS_D0"
// #define WIFI_PASS "FFFFFFFFFF1"

// HTTP
const String URL = "http://happa-26-backend.an.r.appspot.com/plantStats/";
const String PLANT_ID = "LKZvyihQuUbrszjk1h1u";

// MQTT
#define MQTT_SERVER "192.168.10.79"
#define TOPIC_TEST "mikako/happa/test"
#define TOPIC_SUBSCRIBE "thom/happa/test"
#define DEVICE_NAME "Mikako-happa"
#define TIME_BETWEEN_MESSAGES 1000 * 5

// ************************************************************* Class
class Plant {
public:
  int lightLevel = 0;
  int soilWaterLevel = 0;
  float temperature = 0.0;
  float humidityLevel = 0.0;

  // Update values
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
    updateSoilWaterLevel();
    updateLightLevel();
    updateHumidityLevel();
    updateTemperature();
  }

  // Change brightness
  void writeLedBrightness(int input) {
    int brightness = 255 / 100 * input;
    Serial.println("[sensor] Brightness: " + String(brightness));
    analogWrite(LED_PIN, brightness);
  }

  void printAll() {
    Serial.println("[sensor] Light Level: " + String(lightLevel));
    Serial.println("[sensor] Soil Water Level: " + String(soilWaterLevel));
    Serial.println("[sensor] Temperature: " + String(temperature) + "~C");
    Serial.println("[sensor] Humidity: " + String(humidityLevel) + "%");
  }
};

// *********************************************** Instances/variables
Plant plantTest;
WiFiClient wifiClient;
HTTPClient http;
PubSubClient mqttClient(wifiClient);

unsigned long lastMsg = 0;
int value = 0;

// ************************************************************** MAIN
void setup() {
  Serial.println("---------- SET UP START ----------");  
  Serial.begin(115200);
  
  // Sensors
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  dht.begin();
  setupAnalogChip();
  
  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  setUpWiFi();
  
  // HTTP
  setupHTTP();
  
  // MQTT
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(callback);
  setupMQTT();
  subscribeTopics();

  Serial.println("----------- SET UP END -----------");
}

void loop() {
  // Sensors
  plantTest.updateAll();
  plantTest.printAll();

  // HTTP
  // postRequest();
  // delay(300000);  // 5 mins

  // MQTT
  // message();
  publishMessage();

  waitDelay(2000);
}

// ********************************************************* Functions
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

// ********************************************** Set up
void setupAnalogChip() {
  if (!analogChip.begin()) {
    Serial.println("[sensor] Failed to initialize analogChip.");
    while (1);
  }
}

void setUpWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    waitDelay(200);
  }
  Serial.print("[Wifi] IP address: ");
  Serial.println(WiFi.localIP());
}

void setupHTTP() {
  http.begin(wifiClient, URL + PLANT_ID);
  http.addHeader("Content-Type", "application/json");
}

void setupMQTT() {
  Serial.println("[MQTT] connecting MQTT...");
  waitDelay(200);

  if (!mqttClient.connect(DEVICE_NAME)) {
    reconnectMQTT();
  }

  Serial.println("[MQTT] connected to MQTT with device: " + String(DEVICE_NAME));
}

// ************************************************ HTTP
// void postRequest() {
//   int httpCode = sendPostRequest();

//   if (httpCode) {
//     Serial.printf("[HTTP] POST status code: %d\n", httpCode);
//     if (httpCode >= 200 && httpCode < 300) {
//       const String& payload = http.getString();
//       Serial.println("[HTTP] received payload:\n<<" + payload + ">>\n");
//     }
//   } else {
//     Serial.printf("[HTTP] POST error: %s\n", http.errorToString(httpCode).c_str());
//   }

//   http.end();
// }

// int sendPostRequest() {
//   String sensorData = buildJSON();
//   int httpCode = http.POST(sensorData);
//   return httpCode;
// }

// String buildJSON() {
//   String sensorData = "{\"soilWaterLevel\":" + String(plantTest.soilWaterLevel)
//                       + ",\"lightLevel\":" + String(plantTest.lightLevel)
//                       + ",\"humidityLevel\":" + String(plantTest.humidityLevel)
//                       + ",\"temperature\":" + String(plantTest.temperature)
//                       + "}";
//   Serial.println("[HTTP] JSON to be sent : " + sensorData);
//   return sensorData;
// }

// ************************************************ MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("[MQTT] message received (" + String(topic) + ")\n");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnectMQTT() {
  while (!mqttClient.connect(DEVICE_NAME)) {
    mqttClient.state();
    Serial.println("[MQTT] connection failed");
    Serial.println("[MQTT] reconnecting");
    waitDelay(200);
  }
}

void subscribeTopics() {
  boolean r = mqttClient.subscribe(TOPIC_SUBSCRIBE);
  Serial.println("[MQTT] subscribed to: " + String(TOPIC_SUBSCRIBE));
}

void publishMessage() {
  Serial.println("[MQTT] publishing topic: " + String(TOPIC_TEST));
  mqttClient.publish(TOPIC_TEST, "---------------mikako/happa/test----------------");
  waitDelay(200);
}
