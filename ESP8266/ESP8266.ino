#include <DHT.h>
#include <Adafruit_ADS1X15.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ****************************************************** Fixed values
// Pins
#define LED_PIN D2
#define DHT_PIN D3
#define PUMP_POWER_PIN D5
#define LIGHT_POWER_PIN A1
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
const String PLANT_ID = "VAlAa3aEtub3qSw7SIjz";  // Thomas: "wdNtSRStxaQU9gc2QWM7"

// MQTT
//#define MQTT_SEVER "broker.mqtt-dashboard.com"
//#define TOPIC "wemos"
//#define TIME_BETWEEN_MESSAGES 1000 * 5

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

  void printAll() {
    Serial.println("Light Level: " + String(lightLevel));
    Serial.println("Soil Water Level: " + String(soilWaterLevel));
    Serial.println("Temperature: " + String(temperature) + "~C");
    Serial.println("Humidity: " + String(humidityLevel) + "%");
  }
};

// *********************************************** Instances/variables
Plant plantTest;
WiFiClient clientEsp;
HTTPClient http;
//PubSubClient clientMqtt(clientEsp);

unsigned long lastMsg = 0;
int value = 0;

// ************************************************************** MAIN
void setup() {
  Serial.begin(115200);
  
  // Pins
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
//  clientMqtt.setServer(MQTT_SERVER, 1883);
//  clientMqtt.setCallback(callback);
}

void loop() {
  plantTest.updateAll();
  plantTest.printAll();

//  checkMQTT();
  
//  message();

  waitDelay(3000);

  // postRequest();
  // delay(300000);
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
    Serial.println("Failed to initialize analogChip.");
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
  http.begin(clientEsp, URL + PLANT_ID);
  http.addHeader("Content-Type", "application/json");
}
//
//void checkMQTT() {
//  // confirm still connected to mqtt server
//  if (!clientMqtt.connected()) reconnect();
//  clientMqtt.loop();
//}

// ************************************************ HTTP
void postRequest() {
  int httpCode = sendPostRequest();

  if (httpCode) {
    Serial.printf("[HTTP] POST status code: %d\n", httpCode);
    if (httpCode >= 200 && httpCode < 300) {
      const String& payload = http.getString();
      Serial.println("received payload:\n<<" + payload + ">>\n");
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
  String sensorData = "{\"soilWaterLevel\":" + String(plantTest.soilWaterLevel)
                      + ",\"lightLevel\":" + String(plantTest.lightLevel)
                      + ",\"humidityLevel\":" + String(plantTest.humidityLevel)
                      + ",\"temperature\":" + String(plantTest.temperature)
                      + "}";
  Serial.println("JSON to be sent : " + sensorData);
  return sensorData;
}

// ************************************************ MQTT
//void callback(char* topic, byte* payload, unsigned int length) {
//  Serial.println("[MQTT] message received (" + String(topic) + ")\n");
//  
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
//
//  // Switch on the LED if an 1 was received as first character (LOW is the voltage level)
//  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);
//  } else {
//    digitalWrite(BUILTIN_LED, HIGH);
//  }
//}
//
//String createClientID() {
//  uint8_t mac[6];
//  WiFi.macAddress(mac);
//  String clientId = "esp-" + macToStr(mac);
//  return clientId;
//}
//
//String macToStr(const uint8_t* mac) {
//  String str;
//  for (int i = 0; i < 6; ++i) {
//    str += String(mac[i], 16);
//    if (i < 5) str += ':';
//  }
//  return str;
//}
//
//void reconnect() {
//  Serial.println("[MQTT] connecting MQTT");
//  
//  while (!clientMqtt.connected()) {
//    Serial.println(".");
//    
//    String clientId = createClientID();
//    clientId += "-";
//    clientId += String(micros() & 0xff, 16);
//
//    if (clientMqtt.connect(clientId.c_str())) {
//      Serial.println("[MQTT] connected");
//
//      // Once connected, publish an announcement
//      clientMqtt.publish(TOPIC, ("connected " + createClientID()).c_str(), true);
//      
//      // and resubscribe
//      String subscription;
//      subscription += String(TOPIC) + "/" + createClientID() + "/in";
//      clientMqtt.subscribe(subscription.c_str() );
//      Serial.println("[MQTT] subscribed to : " + subscription);
//    } else {
//      Serial.println("[MQTT] error, rc=" + String(clientMqtt.state()) + ", wifi=" + String(WiFi.status()));
//      Serial.println("try again in 5 seconds");
//      delay(5000);
//    }
//  }
//}
//
//void message() {
//  long now = millis();
//  
//  if (now - lastMsg > TIME_BETWEEN_MESSAGES) {
//    lastMsg = now;
//    value += 1;
//
//    String payload = "{\"micros\":" + String(micros())
//                     + ",\"counter\":" + String(value)
//                     + ",\"client\":" + createClientID()
//                     + "}";
//    String pubTopic;
//    pubTopic += String(TOPIC) + "/" + createClientID() + "/out";
//
//    Serial.println("Publish topic: " + pubTopic);
//    Serial.println("Publish message: " + payload);
//    clientMqtt.publish((char*) pubTopic.c_str(), (char*) payload.c_str(), true);
//  }
//}
