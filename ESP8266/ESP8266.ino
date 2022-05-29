//library
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//Sensor pins...
//Analog pins
int lightPin = A0;
int soilMoisurePin = A2;
////Digital pins
int tempHumidPin = 2;
//int lightPowerPin = 4;
//int pumpPowerPin = 6;


//sensor reading variables
int lightData;
int soilMoistureData;
unsigned int minLight = 20;
unsigned int maxLight = 1013;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();

  //Create the object that will interface with the temp/humid sensor
  //rht.begin(tempHumidPin);

  //set pin modes
  pinMode(lightPin, INPUT);
  pinMode(soilMoisurePin, INPUT);
  pinMode(tempHumidPin, INPUT);
}

void loop() {
  // Get light level
  printLight();
  delay(1000);

  //Get water level
  printWater();
  delay(1000);
//  
//read humidity
  printHumid();
  delay(1000);
  
//read temperature as Celsius
  printTemp();
  delay(1000);
}

void printLight(){
  float lightData = analogRead(lightPin);
  float out = ((lightData + 20) / maxLight) * 100;
  Serial.print("Light Level:");
  Serial.println(lightData);
  }

void printWater(){
  int average;
  for(int i = 1; i <= 100; i++) {
   average += analogRead(soilMoisurePin);
  }
  Serial.print("Water Level:");
  Serial.println(analogRead(soilMoisurePin));
  }
//
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
