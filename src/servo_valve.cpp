#include <servo_valve.h>
#include "mbed.h"
//#include "arduino.h"

Servo_valve::Servo_valve (const PinName pwmPinOut, char * name): servo_valveO(pwmPinOut) {
    status = false;
    servopos = CLOSE_SERVO_VALVE;
    _name=name;
    servo_valveO.period_ms(20);
    servo_valveO.pulsewidth_us(servopos);
};


void Servo_valve::openAngle(uint16_t val_angle) {
        uint16_t v = ( (val_angle )*(OPEN_SERVO_VALVE - CLOSE_SERVO_VALVE)/(90) + CLOSE_SERVO_VALVE );
        status = true;
        servopos = v;
        servo_valveO.pulsewidth_us(v);

};

void Servo_valve::open() {
        status = true;
        servopos = OPEN_SERVO_VALVE;
        servo_valveO.pulsewidth_us(servopos);

};


void Servo_valve::close() {
    status = false;
    servopos = CLOSE_SERVO_VALVE;
    servo_valveO.pulsewidth_us(servopos);
 };

uint32_t Servo_valve::getstatus() { 
    return status; 
};
uint32_t Servo_valve::getposvalve() { 
  ;  return servopos; 
}

char *  Servo_valve::getname() { 
    return _name ; 
};

void  Servo_valve::setname(char * n) { 
    _name = n; 
};