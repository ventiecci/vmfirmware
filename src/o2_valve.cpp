#include "o2_valve.h"
#include "mbed.h"
 

#define freq 10000000  
o2_valve_ctrl::o2_valve_ctrl(PinName pwmPinOut):_pwm(pwmPinOut) {
    _pwm.write(0.0);     // after creating it have to be off
    _pwm.period(1.0/freq);
 
};
 
void o2_valve_ctrl::close() {
    _pwm.write(0.0);
};
 
void o2_valve_ctrl::open() {
    _pwm.write(1.0);
};     
void o2_valve_ctrl::open(float duty) {
 
    _pwm.write(duty);            
};
