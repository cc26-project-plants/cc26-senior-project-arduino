//library
#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN D2
#define DHTTYPE DHT22

// Set WiFi credentials
//Code Chrysalis
//#define WIFI_SSID "codechrysalis_2.4ghz"
//#define WIFI_PASS "foreverbekind"
//Home
#define WIFI_SSID "ASUS_D0"
#define WIFI_PASS "FFFFFFFFFF1"

//Constants *****************************************************
DHT dht(DHTPIN, DHTTYPE);

//Class creation & object creation (THIS MAY BE WRONG!!)****************************************
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
    soilWaterLevel = value;
  }
  int getSoilWaterLevel(){
  return soilWaterLevel;  
  }
  
//Light Sensor **********************
  void updateLightLevel(){
    lightLevel = analogRead(A0);
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

Plant fakePlant;


//Setup function ***********************************************

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
//Main program *****************************************************************************

void loop() {
  fakePlant.updateAll();
  fakePlant.printAll();
  waitDelay(1000);

  Serial.println("TEST");
}

//Functions **********************************************************************************
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
