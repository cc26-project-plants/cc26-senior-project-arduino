
// analog pins
int lightPin = A0;
int soilWaterPin = A2;

// digital pins
int pumpPowerPin = 4;
int ledPin = 13;

// humidity/temperature pin
#include "DHT.h";
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int lightLevel;
int soilWaterLevel;
float humidity;
float temperature;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // set pin modes
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

// functions
void readLightLevel() {
  lightLevel = analogRead(lightPin);
  
  Serial.print("Light Level: ");
  Serial.println(lightLevel);
}

void readSoilWaterLevel() {
  soilWaterLevel = analogRead(soilWaterPin);
  
  Serial.print("Soil Water Level: ");
  Serial.println(soilWaterLevel);
}

void readHumidityTemperature() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  Serial.print("Humidity Level: ");
  Serial.println(humidity);
  Serial.print("Temperature: ");
  Serial.println(temperature);
}

void writeLedBrightness() {
  int bright = 950;
  int dark = 1000;
  int ledStep = 255 / (dark - bright);
  
  int lightInput = lightLevel > dark ? dark : lightLevel < bright ? bright : lightLevel;
  int brightness = ledStep * (lightInput - bright);
  analogWrite(ledPin, brightness);
}
