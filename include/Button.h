#include <Arduino.h>
#define DEFAULT_LONGPRESS_LEN 25 // Min nr of loops for a long press
#define DELAY 20                 // Delay per loop in ms

/**
 * @brief   EV_NONE:        1 - No press
 *          EV_SHORTPRESS:  2 - Short Press
 *          EV_LONGPRESS:   3 - Long Press
 */
enum
{
    EV_NONE = 0,
    EV_SHORTPRESS,
    EV_LONGPRESS
};

class Button
{
public:
    // Constructor
    Button(int pin, int longpress_len = DEFAULT_LONGPRESS_LEN);

    // Initialization done after construction, to permit static instances
    void init();

    // Handler, to be called in the loop()
    int handle();

protected:
    bool was_pressed;        // previous state
    int pressed_counter;     // press running duration
    const int pin;           // pin to which button is connected
    const int longpress_len; // longpress duration
};

Button::Button(int p, int lp) : pin(p), longpress_len(lp)
{
}

void Button::init()
{
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH); // pull-up
    was_pressed = false;
    pressed_counter = 0;
}

int Button::handle()
{
    int event;
    int now_pressed = !digitalRead(pin);

    if (!now_pressed && was_pressed)
    {
        // handle release event
        if (pressed_counter < longpress_len)
            event = EV_SHORTPRESS;
        else
            event = EV_LONGPRESS;
    }
    else
        event = EV_NONE;

    // update press running duration
    if (now_pressed)
        ++pressed_counter;
    else
        pressed_counter = 0;

    // remember state, and we're done
    was_pressed = now_pressed;
    return event;
}