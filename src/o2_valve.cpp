#include "o2_valve.h"
#include "mbed.h"
 

#define PERIOD_US 100
o2_valve_ctrl::o2_valve_ctrl(PinName pwmPinOut):_pwm(pwmPinOut) {
    _pwm.period_us(PERIOD_US);
    _pwm.write(0.5);     // after creating it have to be off
 
};
 
void o2_valve_ctrl::close() {
    _pwm.write(0.0);
};
 
void o2_valve_ctrl::open() {
    _pwm.write(1.0);
};     
void o2_valve_ctrl::openA(float duty) {
 
    _pwm.write(duty);            
};

void o2_valve_ctrl::openD(int us) {
 
    if (us<(PERIOD_US+1))
        _pwm.pulsewidth_us(us);  
             
};
 