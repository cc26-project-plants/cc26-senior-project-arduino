
// Analog pins
int lightPin = A0;
int soilWaterPin = A2;

// Digital pins
//int tempHumidPin = 2;
//int pumpPowerPin = 4;
int ledPin = 13;

// Humidity Temperature pin
#include "DHT.h";
//#include "DHT_U.h";
//#include "Adafruit_Sensor.h";
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// sensor reading variables
int lightLevel;
int soilWaterLevel;
float humidity;
float temperature;

// unsigned int minLight = 65536;
//unsigned int maxLight = 0;
//float miniTemp = 5505;
//float maxTemp = 0;
//float minHumid = 100;
//float maxHumid = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  //set pin modes
  pinMode(lightPin, INPUT);
  pinMode(soilWaterPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  readLightLevel();
  writeLedBrightness();
  readSoilWaterLevel();
  readHumidityTemperature();
  
  Serial.println();
  delay(1000);
}

void readLightLevel() {
  lightLevel = analogRead(lightPin);
  
//  Serial.print("Light Level: ");
//  Serial.println(lightLevel);
}

void readSoilWaterLevel() {
  soilWaterLevel = analogRead(soilWaterPin);
  
//  Serial.print("Soil Water Level: ");
//  Serial.println(soilWaterLevel);
}

void readHumidityTemperature() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Temperature: ");
  Serial.println(temperature);
}

void writeLedBrightness() {
  // LED 0 - 255
  int bright = 950;
  int dark = 1000;
  int ledStep = 255 / (dark - bright);
  int input = lightLevel > dark ? dark : lightLevel < bright ? bright : lightLevel;

  int brightness = ledStep * (input - bright);
  analogWrite(ledPin, brightness);
  
//  Serial.print("LED Brightness: ");
//  Serial.println(brightness);
}