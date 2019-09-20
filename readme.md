[![Build Status](https://travis-ci.org/killercode/DomLab.svg?branch=master)](https://travis-ci.org/killercode/DomLab)
[![CodeFactor](https://www.codefactor.io/repository/github/killercode/domlab/badge)](https://www.codefactor.io/repository/github/killercode/domlab)

## Repository of my home automation firmwares

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

```
DIP SWITCH
 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 |ON |ON |ON |ON |OFF|OFF|OFF|N/A|

RX/TX SWITCH TX00
```

**Programming the sketch for the ESP8266:**

```
DIP SWITCH
 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 |OFF|OFF|OFF|OFF|ON |ON |ON |N/A|

RX/TX SWITCH TX00
```

**Running ESP8266 with terminal to PC**

```
DIP SWITCH
 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 |OFF|OFF|OFF|OFF|ON |ON |OFF|N/A|

RX/TX SWITCH TX00
```

**Running ESP8266 <-> ATMEGA**

```
DIP SWITCH
 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 |ON |ON |ON |ON |OFF|OFF|OFF|N/A|

RX/TX SWITCH TX03
```

# Serial Protocol Arduino <-> ESP8266
Current protocol is as follows

**First char:**
- '>' command from arduino to esp
- '<' command from esp to arduino
- '!' debug message

**Key Value Separator**
- ':'

**Terminator**
- ';'

**Keys:**
- config -> configuration instructions
- announce -> announce devices on arduino
- device_name -> message from this device

**example messages:**
1-Switch turned on manually:
```
>/home/switch1Confirm:1;
```

2-Switch turned off via mqtt:
```
</home/switch1:0;
```

3-Wifi connected
```
!:Wifi connected;
```

4-Devices request from ESP8266
```
<config:sub;
```

5-Device announce from arduino
```
>announce:/home/switch1;
```

**Disclaimer**
Why use this kind of protocol?
- The answer is simple... easy to debug and/or monitor. I see a lot of binary protocols but they are mustly unreadable... I am doing this for the sake of simplicity

Why not use JSON for it?
- For must of the things I develop, yes. For this project, i guess it too much overkill... I just need to send short sentences via SERIAL! I don't want to be sending a lot of junk on the serial port.