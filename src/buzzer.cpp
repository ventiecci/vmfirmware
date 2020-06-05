#include "buzzer.h"
#include "mbed.h"
 

 
buzzer_ctrl::buzzer_ctrl(PinName pin):_pwm(pin) {
    _pwm.write(0.0);     // after creating it have to be off
};
 
void buzzer_ctrl::nobeep() {
    _pwm.write(0.0);
};
 
/** Beep with given frequency and duration.
 *
 * @param frequency - the frequency of the tone in Hz
 * @param time - the duration of the tone in seconds
 */
     
void buzzer_ctrl::beep(float freq, float time) {
 
    _pwm.period(1.0/freq);
    _pwm.write(0.5);            // 50% duty cycle - beep on
    toff.attach(this,&buzzer_ctrl::nobeep, time);   // time to off
};
