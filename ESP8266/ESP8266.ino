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

  class Plant {
  public:
    int soilWaterLevel;
    int lightLevel;
    float humidLevel;
    float temp;

    //methods
    void updateSoilWaterLevel(int value){
      soilWaterLevel = value;
    }
};

Plant fakePlant;
fakePlant.soilWaterLevel = 1;
fakePlant.lightLevel = 20;
fakePlant.humidLevel = 3.0;
fakePlant.temp = 10.0;
}

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

//Main program *****************************************************************************

void loop() {

  
  Serial.print("Fake plant Soil water level: ");
  Serial.println(fakePlant.soilWaterLevel);
  delay(2000);
  Serial.println("Updating soil water level...");
  fakePlant.updateSoilWaterLevel(500);
  delay(1000);
  Serial.print("Fake plant Soil water level is now: ");
  Serial.println(fakePlant.soilWaterLevel);
  delay(2000);

  
  Serial.println(analogRead(A0));
  printTemp();
  delay(500);
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

void printTemp(){
  float tempC = dht.readTemperature();
  if (isnan(tempC)) {
    Serial.println("Failed to read temp from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");
    }
  }

void printHumid(){
 float humid = dht.readHumidity();
 if (isnan(humid)) {
  Serial.println("Failed to read humidity from DHT sensor!");
 }else {
  Serial.print("Humidity: ");
  Serial.print(humid);
  Serial.println("%");
 }
}

void updateLight(){
   
  
 }
