#include <blower.h>
#include "mbed.h"
//#include "arduino.h"

Blower::Blower (const PinName pwmPinOut): blower(pwmPinOut) {
    m_stopped = true;
    m_speed = DEFAULT_BLOWER_SPEED;
    blower.period_ms(20);
    blower.pulsewidth_us(m_speed);
	
    
};


void Blower::cmH2O(uint16_t inVal)
{
  uint16_t v = ( (inVal - MIN_BLOWER_CMH20)*(MAX_BLOWER_SPEED - MIN_BLOWER_SPEED)/(MAX_BLOWER_CMH20 - MIN_BLOWER_CMH20) + MIN_BLOWER_SPEED );
  speed(v);  

}



void Blower::speed(uint16_t speed) {
    if ((speed > MIN_BLOWER_SPEED-1) && (speed < MAX_BLOWER_SPEED+1)) {
            m_speed = speed;
            blower.pulsewidth_us(speed);
            m_stopped = false;
    }
     else {
        // print error 
    }
};


void Blower::stop() {
    m_stopped = true;
    blower.pulsewidth_us(MIN_BLOWER_SPEED);
 };

uint32_t Blower::getSpeed() { 
    return m_speed; 
}
