#ifndef BUZZER_H
#define BUZZER_H
 
#include "mbed.h"
 
 
class buzzer_ctrl {
 
public:
    buzzer_ctrl (PinName pin);
    void beep (float frequency, float time); //the frequency of the tone in Hz, time - the duration of the tone in seconds
    void nobeep();
 
private :
    PwmOut _pwm;
    Timeout toff;
};
 

#endif
 