#include <Arduino.h>

/**
 * @brief  Enumerator with the status of the cover
 * @note   For now we only support 3 states system
 */
enum
{
    ST_UNKNOWN = 0,
    ST_OPEN,
    ST_CLOSED,
    ST_OPENING,
    ST_CLOSING,
    ST_ERROR
};

/**
 * @brief  Relay State
 */
enum
{
    RL_OFF = 0,
    RL_ON
};

/**
 * @brief  Class representing a Garage Cover, this means that we should configure a relay 2 sensors
 */
class GarageDoor
{

public:
    GarageDoor(int relayPin,
               int sensorClosedPin,
               int sensorOpenPin,
               String mainTopic,
               String commandTopic,
               String positionTopic,
               int state,
               int operation);

    /**
     * @brief  Initialization done after construction, to permit static instances
     * @retval None
     */
    void init();

    /**
     * @brief  Loop handler to keep the status correct
     * @note
     * @retval None
     */
    void handle();

    /**
     * @brief  Get a string with the command topic
     * @retval Command topic
     */
    String getCommandTopic();

    /**
     * @brief  Get the command message to be sent
     * @retval MQTT message to be sent
     */
    String getCommandMsg();

    /**
     * @brief  Get the position mesage to be sent
     * @retval MQTT message to be sent
     */
    String getPositionMsg();

    /**
     * @brief  Get the relay message to be sent
     * @retval MQTT message to be sent
     */
    String getRelayMsg();

    /**
     * @brief  Set the cover callback
     */
    void setCoverCallback(void (*CoverCallback)(String));

    /**
     * @brief  Set the relay callback
     * @note
     * @retval None
     */
    void setRelayCallback(void (*RelayCallback)(String));

    /**
     * @brief  set a relay state
     * @note
     * @param  newstate: New state of the relay
     * @param  feedback: Should we send the feedback to the MQTT broker?
     */
    void setRelayState(int newstate, bool feedback);

    /**
     * @brief  Get the actual relay state
     * @retval relay state
     */
    int getRelayState();

private:
    void (*CoverCallback)(String);
    void (*RelayCallback)(String);
    int relayPin;
    int relayState;
    int relayCounter;
    int sensorClosedPin;
    int sensorOpenPin;
    String mainTopic;
    String commandTopic;
    String positionTopic;
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
                       int s,
                       int o) : relayPin(rPin),
                                sensorClosedPin(sCPin),
                                sensorOpenPin(sOPin),
                                mainTopic(mt),
                                commandTopic(ct),
                                positionTopic(st),
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
    relayState = RL_OFF;
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

String GarageDoor::getCommandTopic()
{
    return mainTopic + commandTopic;
}

String GarageDoor::getCommandMsg()
{
    if (relayState == RL_ON)
    {
        return ">" + mainTopic + commandTopic + ":OPEN;";
    }
    else
    {
        return ">" + mainTopic + commandTopic + ":CLOSE;";
    }
}

String GarageDoor::getPositionMsg()
{
    if (state == ST_CLOSED)
    {
        return ">" + mainTopic + positionTopic + ":0;";
    }
    if (state == ST_OPEN)
    {
        return ">" + mainTopic + positionTopic + ":100;";
    }
    else
    {
        return ">" + mainTopic + positionTopic + ":50;";
    }
}

void GarageDoor::setRelayState(int newState, bool feedback)
{
    if (newState != relayState)
    {
        if (relayState == RL_OFF)
        {

            relayState = RL_ON;
            relayCounter = 200;

            if (operation == NC)
            {
                digitalWrite(relayPin, LOW);
            }
            else
            {
                digitalWrite(relayPin, HIGH);
            }
        }
        else
        {
            relayState = RL_OFF;
            if (operation == NC)
            {
                digitalWrite(relayPin, HIGH);
            }
            else
            {
                digitalWrite(relayPin, LOW);
            }
        }
        if (feedback)
        {
            if (RelayCallback != nullptr)
            {
                (*RelayCallback)(getCommandMsg());
            }
        }
    }
}

int GarageDoor::getRelayState()
{
    return relayState;
}

void GarageDoor::handle()
{

    if (relayCounter > 0)
    {
        relayCounter--;
    }

    // if counter reached 0 shut off the relay
    if (relayCounter == 0 && relayState == RL_ON)
    {
        this->setRelayState(RL_OFF, true);
    }

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
        if (CoverCallback != nullptr)
        {
            (*CoverCallback)(getPositionMsg());
        }
        last_stat = state;
    }
}

void GarageDoor::setCoverCallback(void (*CoverCallback)(String))
{
    this->CoverCallback = CoverCallback;
}

void GarageDoor::setRelayCallback(void (*RelayCallback)(String))
{
    this->RelayCallback = RelayCallback;
}
