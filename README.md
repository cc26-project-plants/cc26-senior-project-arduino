## CC26 Senior Project - Project Plant

**Happa 🌿**  
Happa is an application designed for those who love plants.  

This repository is dedicated to the microcontroller side of this project.  
You can find more details [here](https://github.com/cc26-project-plants/cc26-senior-project/blob/master/README.md).  
  
  
## Built With
### Software
- C++  
- [Arduino](https://www.arduino.cc/)  
- [ESP8266](http://esp8266.net/)  
- [MQTT](https://mqtt.org/)
- [CloudMQTT](https://www.cloudmqtt.com/)
  
### Hardware components
- Photoresistor
- Soil moisture sensor
- Temperature and humidity sensor
- LED
- Water pump
- Power Relay
  
  
## Getting Started: Microcontroller
1. set up [CloudMQTT](https://www.cloudmqtt.com/) account and build a MQTT broker
2. set up WiFi environment
3. set up the HTTP server to send POST requests to
4. create a header file in `ESP8266` folder
5. include all the credentials in the header file
(Don't forget to include the file in `.gitignore`)
  
    
### Project Structure
![architecture](https://github.com/mikako-shirai/dump/blob/master/diagram.png)  
