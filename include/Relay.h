#include<Arduino.h>

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
         * @param  name: name for the relay
         * @param  state: state of the relay
         * @retval Relay object
         */
        Relay(int pin, String name, uint8_t state);

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
        String getName();

        String MQTTSwitchState();

    private:
        String name;
        uint8_t state;  
        String mqttstate;

    protected:
        const int pin;  // pin
        
};

Relay::Relay(int p, String n, uint8_t s) : pin(p), name(n), state(s)
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

String Relay::getName()
{
    return name;
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
    if (newstate.equals("0"))
    {
        state = HIGH;
    }
    else
    {
        state = LOW;
    }
    setState(state);
    
}

String Relay::MQTTSwitchState()
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