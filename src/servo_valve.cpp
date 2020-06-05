#include <servo_valve.h>
#include "mbed.h"
//#include "arduino.h"

Servo_valve::Servo_valve (const PinName pwmPinOut): servo_valveO(pwmPinOut) {
    status = false;
    servopos = CLOSE_SERVO_VALVE;
    servo_valveO.period_ms(20);
    servo_valveO.pulsewidth_us(servopos);
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

