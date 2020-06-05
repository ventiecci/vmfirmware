#ifndef SERVO_VALVE_H
#define SERVO_VALVE_H
#include "mbed.h"
#include <parameter.h>



class Servo_valve{
public:
   Servo_valve (const PinName pwmPinOut); 
   void open();
   void  close();
   uint32_t getstatus();
   uint32_t getposvalve();


private:
    PwmOut servo_valveO;
    bool status;
    uint32_t servopos;
};

#endif