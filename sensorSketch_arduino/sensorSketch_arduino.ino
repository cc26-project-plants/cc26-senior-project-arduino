//library
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// analog pins
int lightPin = A0;
int soilWaterPin = A2;

// digital pins
int tempHumidPin = 2;
// int lightPowerPin = 4;
// int pumpPowerPin = 6;

// variables
float lightData;
float soilWaterData;
float tempC;
float humid;
unsigned int minLight = 20;
unsigned int maxLight = 1013;

//consts
auto& esp = Serial1;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  // esp.begin(9600);
  dht.begin();

  Serial.flush();
  Serial1.flush();

  pinMode(lightPin, INPUT);
  pinMode(soilWaterPin, INPUT);
  pinMode(tempHumidPin, INPUT);
}

void loop() {
  // serial comminication
  Serial1.write(77);

  // read from port 1, send to port 0:
  // if (esp.available()) {
  //   int inByte = esp.read();
  //   Serial.write(inByte);
  // }

  // read from port 0, send to port 1:
  // if (Serial.available()) {
  //   int inByte = Serial.read();
  //   esp.write("Hello from the other side");
  // }
  
  // read light level
  readLight();
  delay(1000);

  // read soil water level
  readWater();
  delay(1000);

  // read humidity level
  readHumid();
  delay(1000);
  
  // read temperature as Celsius
  readTemp();
  delay(1000);
}

void readLight() {
  lightData = analogRead(lightPin);
  float out = ((lightData + 20) / maxLight) * 100;
  Serial.print("Light Level:");
  Serial.println(lightData);
}

void readWater() {
  // int average;
  // for(int i = 1; i <= 100; i++) {
  //   average += analogRead(soilWaterPin);
  // }
  soilWaterData = analogRead(soilWaterPin);
  Serial.print("Water Level:");
  Serial.println(soilWaterData);
}

void readTemp() {
  tempC = dht.readTemperature();
  if (isnan(tempC)) {
    Serial.println("Failed to read temp from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");
  }
}

void readHumid() {
  humid = dht.readHumidity();
  if (isnan(humid)) {
    Serial.println("Failed to read humidity from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humid);
    Serial.println("%");
  }
}
