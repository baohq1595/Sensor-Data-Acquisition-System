# Sensor Data Acquisition System
..* This project is IoT based application.
..* The project is about collecting environment elements (like temperature, humidity, light, …) in arbitrary place, all information will be stored in a server. Users who want to get the data about that place only need to use an application on Windows with a network connection to browse for data that they want.
..* The advantage of this project is the protocol for communication between the sensors data and server, as well as between server and users application is self-defined base on TCP/IP protocol. So it avoids some security threats.

## Application components
..* SensorHub: manages data and operations of sensors, sends sensors data to Server after a period of time.
..* Server: its responsibility is holding all data received from SensorHub and sending data to users when get request from users.
..* ESPClient: user's application runs on PC, user can request Server to send log data. 

## Hardware components
..* Sensors: temperature & humidity sensor DHT11, light sensor BH1750.
..* SensorHub: Kit TI Cortex M3 TM4C123 with wifi module ESP8266 (connect via UART).
..* Server: Kit TI Cortex M4 TM4C1294 with wifi module ESP8266 (connect via UART).

## Development platform
..* IDE: Code Composer Studio 6.0 (CCS 6.0).
..* ESP8266: AT command.

## Source structure

### Server
..* utils: library for UART0 (from TI CCS 6.0)
..* functions.h/functions.c: functions that implements AT commands of ESP8266.
..* timer.h/timer.c: use timer on the kit.
..* package.h/package.c: implements package specification to exchange between SensorHub <-> Server <-> ESPClient.
..* tm4c1294ncpdt*: config file for the dev kit.
..* main.c: implements protocol to exchange data SensorHub <-> Server <-> ESPClient.

### SensorHub
..* utils: library for UART0 (from TI CCS 6.0)
..* functions.h/functions.c: functions that implements AT commands of ESP8266.
..* timer.h/timer.c: use timer on the kit.
..* package.h/package.c: implements package specification to exchange between SensorHub <-> Server <-> ESPClient.
..* tm4c123gh6pm*: config file for the dev kit.
..* BH1750.h/BH1750.c: implements data reading from the BH1750 sensor.
..* DHT11.h/DHT11.c: implements data reading from the DHT11 sensor.
..* main.c: implements protocol to exchange data SensorHub <-> Server and get data from sensors.

### ESPClient
..* ClientSide.java: implements functions of application and protocol to exchange data between Server <-> ESPClient.
..* Package.java: implements package specification to exchange between SensorHub <-> Server <-> ESPClient.
..* Tag.java: define some constants used in application.
..* MainWindows: GUI of application

## Compile
According to tutorial about how to create a project in CCS IDE.
