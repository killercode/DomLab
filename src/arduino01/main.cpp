/**
 * @author Diogo Alves
 */
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Button.h"
#include <Relay.h>
#include <GarageDoor.h>

#define PIN_SWITCH_1 22
#define PIN_SWITCH_2 24
#define PIN_SWITCH_3 26
#define PIN_SWITCH_4 28
#define PIN_SWITCH_5 30
#define PIN_SWITCH_6 32
#define PIN_SWITCH_7 34
#define PIN_SWITCH_8 36

#define INPUT1 53
#define INPUT2 51
#define INPUT3 49

// handler for garage MQTT messages
void CoverCallback(String command);
void RelayCallback(String command);

String inString;

// Define buttons
Button button1(INPUT1, DEFAULT_LONGPRESS_LEN * 2);
Button button2(INPUT2, DEFAULT_LONGPRESS_LEN * 2);
Button button3(INPUT3, DEFAULT_LONGPRESS_LEN * 2);

// Define relays
Relay out[7] = {
    Relay(PIN_SWITCH_1, "/house/switch1/", "command/", "state/", HIGH, NC),
    Relay(PIN_SWITCH_2, "/house/switch2/", "command/", "state/", HIGH, NC),
    Relay(PIN_SWITCH_3, "/house/switch3/", "command/", "state/", HIGH, NC),
    Relay(PIN_SWITCH_4, "/house/switch4/", "command/", "state/", HIGH, NC),
    Relay(PIN_SWITCH_5, "/house/switch5/", "command/", "state/", HIGH, NC),
    Relay(PIN_SWITCH_6, "/house/switch6/", "command/", "state/", HIGH, NC),
    Relay(PIN_SWITCH_7, "/house/switch7/", "command/", "state/", HIGH, NC),
    //Relay(PIN_SWITCH_8, "/house/switch8/", "command/", "state/", HIGH, NC),
};

// Define Garage doors
GarageDoor garage1 = GarageDoor(PIN_SWITCH_8, 2, 3, "/house/garage1/", "cover/set", "cover/position", 0, NC);

/**
 * @brief  Trigget button events
 * @param  *button_name: Name of the button that triggered the event
 * @param  event: 1 short press, 2 long press
 * @retval None
 */
void button_event(const char *button_name, int event)
{
    if (event)
    {
        if (event > 0)
        {
            int bNum = String(button_name).toInt();
            if (bNum == 1)
            {
                garage1.setRelayState(RL_ON, true);
                //Serial3.print(garage1.getCommandMsg());
                //out[0].SwitchState();
                //out[1].SwitchState();
                //Serial3.print(out[0].getCommandMsg());
                //Serial3.flush();
                //Serial3.print(out[1].getCommandMsg());
                //Serial3.flush();
            }
            else if (bNum == 2)
            {
                out[2].SwitchState();
                out[3].SwitchState();
                Serial3.print(out[2].getCommandMsg());
                Serial3.flush();
                Serial3.print(out[3].getCommandMsg());
                Serial3.flush();
            }
            else if (bNum == 3)
            {
                out[4].SwitchState();
                out[5].SwitchState();
                Serial3.print(out[4].getCommandMsg());
                Serial3.flush();
                Serial3.print(out[5].getCommandMsg());
                Serial3.flush();
            }
            else
            {
                // TODO add handler for switches to allow momentary buttons
            }
        }
    }
}

/**
 * @brief  Announce the devices to ESP8266
 * @retval None
 */
void announceToESP()
{
    for (int i = 0; i < 7; i++)
    {
        Serial3.print(">announce:" + out[i].getCommandTopic() + ";");
        Serial3.flush();
    }

    Serial3.print(">announce:" + garage1.getCommandTopic() + ";");
    Serial3.flush();

    garage1.setCoverCallback(CoverCallback);
    garage1.setRelayCallback(RelayCallback);
}

/**
 * @brief  After receive a message we should update the device status
 * @param  channel: topic / name of the device that changed
 * @param  action: What was the change
 * @retval None
 */
void updateDeviceStatus(String channel, String action)
{
    for (int i = 0; i < 7; i++)
    {
        if (out[i].getCommandTopic().equals(channel))
        {
            out[i].setStringState(action);
            Serial3.print(out[i].getStateMsg());
            Serial3.flush();
            break;
        }
    }

    if (garage1.getCommandTopic().equals(channel))
    {
        if (action.equals("OPEN"))
        {
            Serial.flush();
            garage1.setRelayState(RL_ON, false);
        }
    }
}

/**
 * @brief  Handles the received messages from serial communications
 * @retval None
 */
void SerialHandler()
{
    while (Serial3.available())
    {
        char inChar = Serial3.read();

        if ((inChar != '\n') && (inChar != '\r'))
        {
            inString += inChar;
        }

        if (inString.length() > 255)
        {
            Serial.println("ARDUINO: MAX BUFFER REACHED [" + inString + "]");
            inString = "";
        }

        if (inChar == ';')
        {
            if (inString.indexOf('<') >= 0)
            {
                int cstart = 1;
                int cend = inString.indexOf(':');

                int sstart = inString.indexOf(':') + 1;
                int send = inString.indexOf(';');

                if (inString.substring(cstart, cend).equals("config"))
                {
                    if (inString.substring(sstart, send).equals("announce"))
                    {
                        announceToESP();
                    }
                }
                else
                {
                    String channel = inString.substring(cstart, cend);
                    String action = inString.substring(sstart, send);
                    Serial.println("DEBUG: Received update - " + channel + ":" + action + ";");
                    updateDeviceStatus(channel, action);
                }
            }
            else if (inString.indexOf('!') >= 0)
            {
                Serial.println("DEBUG ESP" + inString + ";");
                Serial.flush();
            }
            else
            {
                Serial.println("DEBUG received a wrong command: [" + inString + "];");
                Serial.flush();
            }

            inString = "";
        }
    }
}

/**
 * @brief  Setup
 * @retval None
 */
void setup()
{

    Serial.begin(115200);
    Serial3.begin(115200);

    pinMode(PIN_SWITCH_1, OUTPUT);
    pinMode(PIN_SWITCH_2, OUTPUT);
    pinMode(PIN_SWITCH_3, OUTPUT);
    pinMode(PIN_SWITCH_4, OUTPUT);
    pinMode(PIN_SWITCH_5, OUTPUT);
    pinMode(PIN_SWITCH_6, OUTPUT);
    pinMode(PIN_SWITCH_7, OUTPUT);
    //pinMode(PIN_SWITCH_8, OUTPUT);

    for (int i = 0; i < 7; i++)
    {
        out[i].init();
    }

    // init buttons pins; I suppose it's best to do here
    button1.init();
    button2.init();
    button3.init();

    garage1.init();

    delay(20);
}

/**
 * @brief  Device Loop
 * @retval None
 */
void loop()
{
    // do other things
    int event1 = button1.handle();
    int event2 = button2.handle();
    int event3 = button2.handle();

    button_event("1", event1);
    button_event("2", event2);
    button_event("3", event3);

    garage1.handle();

    SerialHandler();

    delay(20);
}

void CoverCallback(String message)
{
    Serial.println(message);
    Serial.flush();
    Serial3.println(message);
    Serial3.flush();
    //delay(20);
}

void RelayCallback(String message)
{
    //Serial.println(message);
    //Serial.flush();
    Serial3.println(message);
    Serial3.flush();
}