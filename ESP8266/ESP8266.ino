//ESP library
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

// Set WiFi credentials
//Code Chrysalis
#define WIFI_SSID "codechrysalis_2.4ghz"
#define WIFI_PASS "foreverbekind"

//Home
//#define WIFI_SSID "ASUS_D0"
//#define WIFI_PASS "FFFFFFFFFF1"

//DHT Sensor
#include "DHT.h"
#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//Analog Sensors
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 analogChip;

//Wifi Client *****************************************************
WiFiClient wifiClient;
//HTTP Request Client
HTTPClient http;

//MQTT*************************************************************
#include <PubSubClient.h>
#define MQTT_SEVER "test.mosquitto.org"
PubSubClient mqttClient(wifiClient);
#define TOPIC "cc26/happa/sensors"
#define TIME_BETWEEN_MESSAGES 1000 * 5
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

//Define pins
const int pumpPower = D5;
const int lightPower = D6;

//Class creation & object creation *****************************************************************************************
//**************************************************************************************************************************
class Plant;
class Plant {
public:
  int soilWaterLevel = 0;
  int lightLevel = 0;
  float humidityLevel = 0.0;
  float temperature = 0.0;

//methods *****************************************
//Soil Water Sensor **********************
  void updateSoilWaterLevel(int value){
    soilWaterLevel = analogChip.readADC_SingleEnded(1);
  }
  int getSoilWaterLevel(){
    return soilWaterLevel;  
  }
  
//Light Sensor **********************
  void updateLightLevel(){
    lightLevel = analogChip.readADC_SingleEnded(0);
  }
  int getLightLevel(){
  return lightLevel;  
  }

//Humidity Sensor **********************
  void updateHumidityLevel(){
    humidityLevel = dht.readHumidity();
  }
  int getHumidityLevel(){
  return humidityLevel;  
  }

//temp Sensor **********************
  void updateTemperature(){
    temperature = dht.readTemperature();
  }
  int getTemperature(){
  return temperature;  
  }

//General methods *****************************
  void printAll(){
    Serial.print("Light Level: ");
    Serial.println(lightLevel);

    Serial.print("Soil Water Level: ");
    Serial.println(soilWaterLevel);

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("~C");

    Serial.print("Humidity: ");
    Serial.print(temperature);
    Serial.println("%");
  }

  void updateAll(){
      updateSoilWaterLevel(333);
      updateLightLevel();
      updateHumidityLevel();
      updateTemperature();
  }
  
};

//Declare instance of plant class
Plant fakePlant;


//Setup function ***********************************************
//**************************************************************

void setup() {
  // Setup serial port
  Serial.begin(115200);
  
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

  //ANALOG INIT
  if (!analogChip.begin()) {
    Serial.println("Failed to initialize analogChip.");
    while (1);
  }

//  pinMode(pumpPower, OUTPUT);
//  pinMode(lightPower, OUTPUT);
//  digitalWrite(pumpPower, LOW);
//  digitalWrite(pumpPower, LOW);
}
//Main program *****************************************************************************
//******************************************************************************************

void loop() {
 
  fakePlant.updateAll();
  fakePlant.printAll();
  waitDelay(1000);


//  Serial.println("Sending post request...");
//  postRequest();
////Delay for 1 hour
//  delay(3600000);
}

//End main program*************************************************************************************************************
//*****************************************************************************************************************************

//Functions **********************************************************
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

void postRequest(){
  String jString = "{\"soilWaterLevel\":" + String(fakePlant.soilWaterLevel) + ",\"lightLevel\":" + String(fakePlant.lightLevel) + ",\"humidityLevel\":" + String(fakePlant.humidityLevel) + ",\"temperature\":" + String(fakePlant.temperature) + "}";
  
  Serial.print("JSON string to be sent");
  Serial.println(jString);
  
  http.begin(wifiClient, "http://happa-26-backend.an.r.appspot.com/plantStats/wdNtSRStxaQU9gc2QWM7");
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jString);
  if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  http.end();
}
