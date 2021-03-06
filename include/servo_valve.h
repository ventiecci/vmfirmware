#ifndef SERVO_VALVE_H
#define SERVO_VALVE_H
#include "mbed.h"
#include <parameter.h>



class Servo_valve{
public:
   Servo_valve (const PinName pwmPinOut,char * name);
   void open();
   void close();
   void openAngle(uint16_t val_angle);

   uint32_t getstatus();
   uint32_t getposvalve();

   char *  getname();
   void  setname(char * n);


private:
    PwmOut servo_valveO;
    bool status;
    uint32_t servopos;
    char * _name;
 
};

#endif