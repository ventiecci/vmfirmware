#ifndef BLOWER_H
#define BLOWER_H
#include "mbed.h"
//#include "arduino.h"
#include <parameter.h>


class Blower{
public:
    Blower (const PinName pwmPinOut, char * name);
    void speed(uint16_t speed);
    void stop();
    uint32_t getSpeed();
    void cmH2O(uint16_t inVal);
    char *  getname();
    void  setname(char * n);

private:
    PwmOut blower;
    bool m_stopped;
    uint32_t m_speed;
    char * _name;
 

};

#endif