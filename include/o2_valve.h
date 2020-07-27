
#ifndef O2_VALVE_H
#define O2_VALVE_H
#include "mbed.h"
#include <parameter.h>



class o2_valve_ctrl{
public:
   o2_valve_ctrl (const PinName pwmPinOut); 
   void close();
   void open();
   void openA(float duty);
   void openD(int us);

private:
    PwmOut _pwm;
};

#endif