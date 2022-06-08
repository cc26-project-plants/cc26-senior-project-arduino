//ESP library
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <millisDelay.h>

// Set WiFi credentials
//Code Chrysalis
//#define WIFI_SSID "codechrysalis_2.4ghz"
//#define WIFI_PASS "foreverbekind"

//Home
#define WIFI_SSID "ASUS_D0"
#define WIFI_PASS "FFFFFFFFFF1"

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
PubSubClient mqttClient(wifiClient);
#define TOPIC "thom/happa/test"

//Define pins
const int pumpPower = D5;
const int lightPower = D6;

//constants
const int maxDry = 17100;
const int maxWet = 9360;

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
  void updateSoilWaterLevel(){
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
    Serial.print(humidityLevel);
    Serial.println("%");
  }

  void updateAll(){
      updateSoilWaterLevel();
      updateLightLevel();
      updateHumidityLevel();
      updateTemperature();
  }

//POWER Methods
  void lightOn(){
    Serial.println("Turning light on...");
    digitalWrite(lightPower, HIGH);
    mqttClient.publish("light/status", "on");
  }

  void lightOff(){
    Serial.println("Turning light off...");
    digitalWrite(lightPower, LOW);
    mqttClient.publish("light/status", "off");
  }

  void water(){
    Serial.println("Watering plant...");
    digitalWrite(pumpPower, HIGH);
    delay(2000);
    Serial.println("Finished watering plant");
  }
  
};

//Declare instance of plant class
Plant fakePlant;
millisDelay postAllSensorDelay;
millisDelay printPlantDelay;
millisDelay autoWaterDelay;
millisDelay mqttConnectDelay;

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
    Serial.println("Attempting Wifi connection");
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

  //Connect to MQTT broker
  mqttConnect();

  pinMode(pumpPower, OUTPUT);
  pinMode(lightPower, OUTPUT);
  digitalWrite(lightPower, LOW);
//  digitalWrite(pumpPower, LOW);
//  digitalWrite(pumpPower, LOW);
  postAllSensorDelay.start(1000 * 30);
  printPlantDelay.start(1000);
  autoWaterDelay.start(1000);
  mqttConnectDelay.start(100);
}
//Main program *****************************************************************************
//******************************************************************************************

void loop() {
  //Non-blocking delay for post request once an hour
  if (postAllSensorDelay.justFinished()){
    fakePlant.updateAll();
    Serial.println("Sending post request...");
    postRequest();
    postAllSensorDelay.start(1000 * 60 * 60); //once an hour
  }

  //Non-blocking delay to print plant stats every thirty seconds
  if (printPlantDelay.justFinished()){
    fakePlant.updateAll();
    fakePlant.printAll();
    Serial.println("");
    printPlantDelay.start(30 * 1000); //every thirty seconds
  }

  //If the plant doesn't have enough water, water it and wait 10 minutes to see if it needs more water
  if (autoWaterDelay.justFinished() && fakePlant.getSoilWaterLevel() > 16000){
      fakePlant.water();
      autoWaterDelay.start(1000 * 60 * 10);
  }

  if (!mqttClient.connected() && mqttConnectDelay.justFinished()){
    mqttConnect();
    mqttConnectDelay.start(1000 * 60 * 5);
  }

  mqttClient.loop();
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

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg = msg + (char)payload[i];
    Serial.print((char)payload[i]);
  }

  mqttLogic(String(topic), msg);
  
  Serial.println("");
  Serial.println(msg);
}

void mqttLogic(String topic, String msg){

  if(topic == "light/request") {
    if(msg == "1"){
      fakePlant.lightOn();
    } 

    if(msg == "0"){
      fakePlant.lightOff();
    }  
  }else if (topic == "water/request"){
    if(msg == "1"){
      fakePlant.water();
    } 
  }
}

void mqttConnect(){
  mqttClient.setServer("192.168.10.79", 1883);
  //Set callback function for recieved MQTT messages
  mqttClient.setCallback(mqttCallback);
  //Establish connection

  waitDelay(100);
  if (mqttClient.connect("Thom-happa")) {
    // connection succeeded
    Serial.println("Connected to MQTT Broker");
    boolean r= mqttClient.subscribe("mikako/happa/test");
    mqttClient.subscribe("light/request");
    Serial.println("Subscribed to mikako/happa/test");

  } 
  else {
    // connection failed
    Serial.println(mqttClient.state());
    // will provide more information
    // on why it failed.
    Serial.println("Connection failed ");
  }
}
