//Sensor pins...
//Analog pins
int lightPin = A0;
int soilMoisurePin = A2;
//Digital pins
int tempHumidPin = 2;
int lightPowerPin = 4;
int pumpPowerPin = 6;


//sensor reading variables
int lightData;
int soilMoistureData;
unsigned int minLight = 65536;
unsigned int maxLight = 0;
float miniTemp = 5505;
float maxTemp = 0;
float minHumid = 100;
float maxHumid = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Create the object that will interface with the temp/humid sensor
  //rht.begin(tempHumidPin);

  //set pin modes
  pinMode(lightPin, INPUT);
  pinMode(soilMoisurePin, INPUT);
}

void loop() {
  // photoresistor code
  //Get light reading
  lightData = analogRead(lightPin);
  soilMoistureData = analogRead(soilMoisurePin);
  Serial.print("Light Level:");
  Serial.println(lightData);
  Serial.print("Water Level:");
  Serial.println(soilMoistureData);
  delay(1000);
}
