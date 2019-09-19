#include<Arduino.h>


//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
// Class definition

class Relay 
{
    public:
        // Constructor
        Relay(int pin, String name, bool state);

        // Initialization done after construction, to permit static instances
        void init();

        
        String DoSwitch();

        // Set State
        void SetState(bool newState);

        String getName();

    private:
        bool state;           // previous state
        String name;

    protected:
        const int pin;        // pinState
};

Relay::Relay(int p, String n, bool s) : pin(p), name(n), state(s)
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