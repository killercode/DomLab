#include <Arduino.h>

enum
{
    ST_UNKNOWN = 0,
    ST_OPEN,
    ST_CLOSED,
    ST_OPENING,
    ST_CLOSING,
    ST_ERROR
};

class GarageDoor
{

public:
    GarageDoor(int relayPin,
               int sensorClosedPin,
               int sensorOpenPin,
               String mainTopic,
               String commandTopic,
               String positionTopic,
               String setPositionTopic,
               int state,
               int operation);

    /**
     * @brief  Initialization done after construction, to permit static instances
     * @retval None
     */
    void init();

    void handle();

    void setCallback(void (*callback)(String));

    String getCommandMsg();
    String getStateMsg();

private:
    void (*callback)(String);
    int relayPin;
    int sensorClosedPin;
    int sensorOpenPin;
    String mainTopic;
    String commandTopic;
    String positionTopic;
    String setPositionTopic;
    int state;
    int operation;
    int last_stat;
    int counter;

protected:
    const int duration = 500;
};

GarageDoor::GarageDoor(int rPin,
                       int sCPin,
                       int sOPin,
                       String mt,
                       String ct,
                       String st,
                       String spt,
                       int s,
                       int o) : relayPin(rPin),
                                sensorClosedPin(sCPin),
                                sensorOpenPin(sOPin),
                                mainTopic(mt),
                                commandTopic(ct),
                                positionTopic(st),
                                setPositionTopic(spt),
                                state(s),
                                operation(o)
{
}

void GarageDoor::init()
{
    counter = 0;
    pinMode(relayPin, OUTPUT);
    pinMode(sensorClosedPin, INPUT_PULLUP);
    pinMode(sensorOpenPin, INPUT_PULLUP);

    if (operation == NC)
    {
        digitalWrite(relayPin, HIGH); // pull-up
    }
    else
    {
        digitalWrite(relayPin, LOW); // pull-up
    }

    if (digitalRead(sensorClosedPin))
    {
        state = ST_CLOSED;
    }
    else if (digitalRead(sensorOpenPin))
    {
        state = ST_OPEN;
    }
    else
    {
        state = ST_UNKNOWN;
    }
}

String GarageDoor::getCommandMsg()
{
    return ">" + mainTopic + commandTopic + ":OPEN;";
}

String GarageDoor::getStateMsg()
{
    if (state == ST_CLOSED)
    {
        return ">" + mainTopic + setPositionTopic + ":0;";
    }
    if (state == ST_OPEN)
    {
        return ">" + mainTopic + setPositionTopic + ":100;";
    }
    else
    {
        return ">" + mainTopic + setPositionTopic + ":50;";
    }
}

void GarageDoor::handle()
{

    bool closed = digitalRead(sensorClosedPin) == LOW;
    bool open = digitalRead(sensorOpenPin) == LOW;

    if (closed && !open)
    {
        state = ST_CLOSED;
    }
    else if (!closed && open)
    {
        state = ST_OPEN;
    }
    else if (open && closed)
    {
        // This should never happen
        state = ST_ERROR;
    }
    else
    {
        if (last_stat == ST_CLOSED)
        {
            state = ST_OPENING;
            counter = 0;
        }
        if (last_stat == ST_OPEN)
        {
            state = ST_CLOSING;
            counter = 0;
        }
    }

    if ((last_stat == ST_OPENING) && (state == last_stat))
    {
        counter = counter + 1;
    }

    if ((last_stat == ST_CLOSING) && (state == last_stat))
    {
        counter = counter + 1;
    }

    if (counter > 5000)
    {
        counter = 0;
        state = ST_UNKNOWN;
    }

    if (last_stat != state)
    {
        if (state == ST_OPEN)
        {
            Serial.println("DOOR OPEN");
        }
        else if (state == ST_CLOSED)
        {
            Serial.println("DOOR CLOSED");
        }
        else if (state == ST_OPENING)
        {
            Serial.println("DOOR OPENING");
        }
        else if (state == ST_CLOSING)
        {
            Serial.println("DOOR CLOSING");
        }
        else if (state == ST_UNKNOWN)
        {
            Serial.println("UNKNOWN");
        }
        (*callback)(getStateMsg());
        last_stat = state;
    }
}

void GarageDoor::setCallback(void (*callback)(String))
{
    this->callback = callback;
}
