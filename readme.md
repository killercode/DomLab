[![Build Status](https://travis-ci.org/killercode/DomLab.svg?branch=master)](https://travis-ci.org/killercode/DomLab)

##Repository of my home automation firmwares

I am using an arduino mega clone with a ESP8266 onboard connected to the arduino mega serial

# Main idea
Have a serial protocol to make the firmware on ESP8266 generic enought to be able to receive identification of each sensor or actuator and subscribe the MQTT topics that are relevant to these components.

This way we should only need to configure each arduino about what components.

For each components I will create a class that will handle the naming, pin I/O and all the inherent logic, that way we only have to deal with instancianting the classes and assign them to the correct pinout and that's all the code needed to the system to work


# Setup
For the first module I will have a board mentioned above with a 8 Relay Board

To understand how to flash the arduino or the ESP we should check the following diagram

# Setting up board

**Programming the sketch for the ATMEGA:**

```DIP SWITCH
 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 |ON |ON |ON |ON |OFF|OFF|OFF|N/A|

RX/TX SWITCH TX00
```

**Programming the sketch for the ESP8266:**

```DIP SWITCH
 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 |OFF|OFF|OFF|OFF|ON |ON |ON |N/A|

RX/TX SWITCH TX00
```

**Running ESP8266 with terminal to PC**

```DIP SWITCH
 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 |OFF|OFF|OFF|OFF|ON |ON |OFF|N/A|

RX/TX SWITCH TX00
```

**Running ESP8266 <-> ATMEGA**

```DIP SWITCH
 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 |ON |ON |ON |ON |OFF|OFF|OFF|N/A|

RX/TX SWITCH TX03
```