/**
 * @author Diogo Alves
 */
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Button.h"
#include <Relay.h>

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

String inString;

ButtonHandler button1(INPUT1, DEFAULT_LONGPRESS_LEN * 2);
ButtonHandler button2(INPUT2, DEFAULT_LONGPRESS_LEN * 2);
ButtonHandler button3(INPUT3, DEFAULT_LONGPRESS_LEN * 2);

Relay out[8] = {
    Relay(PIN_SWITCH_1, "/house/switch1", HIGH),
    Relay(PIN_SWITCH_2, "/house/switch2", HIGH),
    Relay(PIN_SWITCH_3, "/house/switch3", HIGH),
    Relay(PIN_SWITCH_4, "/house/switch4", HIGH),
    Relay(PIN_SWITCH_5, "/house/switch5", HIGH),
    Relay(PIN_SWITCH_6, "/house/switch6", HIGH),
    Relay(PIN_SWITCH_7, "/house/switch7", HIGH),
    Relay(PIN_SWITCH_8, "/house/switch8", HIGH),
};

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
                Serial3.print(">" + out[0].getName() + "Confirm/:" + out[0].MQTTSwitchState() + ";");
                Serial3.print(">" + out[1].getName() + "Confirm/:" + out[1].MQTTSwitchState() + ";");
            }
            else if (bNum == 2)
            {
                Serial3.print(">" + out[2].getName() + "Confirm/:" + out[2].MQTTSwitchState() + ";");
                Serial3.print(">" + out[3].getName() + "Confirm/:" + out[3].MQTTSwitchState() + ";");
            }
            else if (bNum == 3)
            {
                Serial3.print(">" + out[4].getName() + "Confirm/:" + out[4].MQTTSwitchState() + ";");
                Serial3.print(">" + out[5].getName() + "Confirm/:" + out[5].MQTTSwitchState() + ";");
            }
            else
            {
                Serial3.print(">" + out[6].getName() + "Confirm/:" + out[6].MQTTSwitchState() + ";");
                Serial3.print(">" + out[7].getName() + "Confirm/:" + out[7].MQTTSwitchState() + ";");
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
    for (int i = 0; i < 8; i++)
        Serial.print(">announce:" + out[i].getName());
}

/**
 * @brief  After receive a message we should update the device status
 * @param  channel: topic / name of the device that changed
 * @param  action: What was the change
 * @retval None
 */
void updateDeviceStatus(String channel, String action)
{
    for (int i = 0; i < 8; i++)
    {
        if (out[i].getName().equals(channel))
        {
            out[i].setStringState(action);
            Serial3.print(">" + out[i].getName() + "Confirm/:" + action + ";");
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
            Serial3.flush();
            inString = "";
        }

        if (inChar == ';')
        {
            if (inString.indexOf('<') >= 0)
            {
                int cstart = 1;
                int cend = inString.indexOf(':');
                int csize = cend - cstart;

                int sstart = inString.indexOf(':') + 1;
                int send = inString.indexOf(';');
                int ssize = send - sstart;

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

                    updateDeviceStatus(channel, action);
                }
            }
            else if (inString.indexOf('!') >= 0)
            {
                Serial.println("DEBUG" + inString);
            }
            else
            {
                Serial.println("DEBUG received a wrong command: [" + inString + "];");
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
    pinMode(PIN_SWITCH_8, OUTPUT);

    for (int i = 0; i < 8; i++)
    {
        out[i].init();
    }

    // init buttons pins; I suppose it's best to do here
    button1.init();
    button2.init();
    button3.init();

    announceToESP();
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

    SerialHandler();

    delay(20);
}