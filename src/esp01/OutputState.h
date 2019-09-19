#include<Arduino.h>


//////////////////////////////////////////////////////////////////////////////
// Simple class to manage a relay
//////////////////////////////////////////////////////////////////////////////
// Class definition

class Relay 
{
    public:
        // Constructor
        Relay(int pin, String name, uint8_t state);

        // Initialization done after construction, to permit static instances
        void init();

        
        String switchState();

        // Get State
        uint8_t getState();

        // Set State
        void SetState(uint8_t newState);

        String getName();

    private:
        String name;
        uint8_t state;  // state  

    protected:
        const int pin;  // pin
        
};

Relay::Relay(int p, String n, uint8_t s) : pin(p), name(n), state(s)
{
}

void Relay::init()
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); // pull-up
  state = false;  
}

String Relay::DoSwitch()
{
    state = !state;

    if (state)
    {
        digitalWrite(pin, HIGH);
        return "0";
    }
    else
    {
        digitalWrite(pin, LOW);
        return "1";
    }
}

void Relay::SetState(bool newState)
{
    state= newState;
    if (state)
    {
        digitalWrite(pin, HIGH);
    }
    else
    {
        digitalWrite(pin, LOW);
    }
}

String Relay::getName()
{
    return name;
}

//////////////////////////////////////////////////////////////////////////////