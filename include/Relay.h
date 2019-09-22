#include <Arduino.h>

/**
 * @brief   NC - Normally closed
 *          NO - Normally open
 * @note
 * @retval None
 */
enum
{
    NC = 0,
    NO
};
/**
 * @brief  Relay class to easier the operations
 * @note   IMPORTANT: MQTT Logic is inverted!
 * @retval None
 */
class Relay
{

public:
    /**
     * @brief  Constructor
     * @param  pin: pin where the relay is connected
     * @param  mainTopic: maintopic for the relay
     * @param  commandTopic: commandTopic for the relay
     * @param  stateTopic: stateTopic for the relay
     * @param  state: state of the relay
     * @retval Relay object
     */
    Relay(int pin, String mainTopic, String commandTopic, String stateTopic, uint8_t state, int operation);

    /**
     * @brief  Initialization done after construction, to permit static instances
     * @retval None
     */
    void init();

    /// Get State
    /**
     * @brief  Get the actual state of the relay
     * @retval 1 or 0
     */
    uint8_t getState();

    /**
     * @brief  Set the state of the relay
     * @note   this will actually update it physically
     * @param  newState: new state to be set
     * @retval None
     */
    void setState(uint8_t newState);

    /**
     * @brief  Get MQTT friendly state
     * @note
     * @retval "1" or "0"
     */
    String getStringState();

    /**
     * @brief  set MQTT friendly state
     * @note
     * @retval None
     */
    void setStringState(String newstate);

    /**
     * @brief  Get the relay name as a MQTT topic
     * @note
     * @retval mqtt topic for the device
     */
    //String getName();

    String SwitchState();

    String getCommandMsg();
    String getStateMsg();

    String getStateTopic();
    String getCommandTopic();

private:
    uint8_t state;
    int operation;
    String mainTopic;
    String commandTopic;
    String stateTopic;

protected:
    const int pin; // pin
};

Relay::Relay(int p, String m, String ct, String st, uint8_t s, int o) : pin(p), mainTopic(m), commandTopic(ct), stateTopic(st), state(s), operation(o)
{
}

void Relay::init()
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state); // pull-up
}

void Relay::setState(uint8_t newState)
{
    state = newState;
    digitalWrite(pin, state);
}

uint8_t Relay::getState()
{
    return state;
}

String Relay::getStringState()
{
    if (state == 0)
    {
        return "1";
    }
    else
    {
        return "0";
    }
}

void Relay::setStringState(String newstate)
{
    if (operation == NC)
    {
        if (newstate.equals("0"))
        {
            state = HIGH;
        }
        else
        {
            state = LOW;
        }
    }
    else
    {
        if (newstate.equals("0"))
        {
            state = LOW;
        }
        else
        {
            state = HIGH;
        }
    }

    setState(state);
}

String Relay::SwitchState()
{
    if (operation == NC)
    {
        if (state == LOW)
        {
            setState(HIGH);
            return "0";
        }
        else
        {
            setState(LOW);
            return "1";
        }
    }
    else
    {
        if (state == LOW)
        {
            setState(HIGH);
            return "1";
        }
        else
        {
            setState(LOW);
            return "2";
        }
    }
}

String Relay::getCommandMsg()
{
    return ">" + mainTopic + commandTopic + ":" + getStringState() + ";";
}

String Relay::getStateMsg()
{
    return ">" + mainTopic + stateTopic + ":" + getStringState() + ";";
}

String Relay::getStateTopic()
{
    return mainTopic + stateTopic;
}

String Relay::getCommandTopic()
{
    return mainTopic + commandTopic;
}
