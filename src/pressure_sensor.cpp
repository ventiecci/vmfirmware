#include "mbed.h"
//#include "arduino.h"
#include <parameter.h>
#include <pressure_sensor.h>

 pressure_sensor::pressure_sensor (const PinName an):pre_sensor(an){

     
 };
 float pressure_sensor::readRAW(){
        return pre_sensor.read();

 };

 /*
 Output (V) =(0.8 x Vsupply /Pmax – Pmin) x (Pressure_applied – Pmin) +0.10 x Vsupply
 Pressure_applied= (((Output (V))-0.10 x Vsupply)*(Pmax – Pmin)/(0.8 x Vsupply)) -Pmin
 Pressure_applied= ((praw-0.10)*(1psi – PCpsi)/(0.8 x 1)) -PCpsi
 */

#define PRESSURE_COMPENSATED_MIN 0.062
 float pressure_sensor::readPsi(){

       float praw= pre_sensor.read();
       return ((praw-0.1)*(1-PRESSURE_COMPENSATED_MIN)/0.8)-PRESSURE_COMPENSATED_MIN;

 };

/*
 0.014223343334285 psi = 1 cmH2O
*/
 float pressure_sensor::readCMH2O(){

       float ppsi= readPsi();
       return ppsi/0.014;
      
   
 };

bool  pressure_sensor::min_pressure_level(float val){
     if (_min_pressure>(val-2)){
              return 1;
       }       
       return 0;

};
bool  pressure_sensor::max_pressure_level(float val){
       if (_max_pressure<(val+4)){
              return 1;
       }       
       return 0;

};
void  pressure_sensor::set_min_pressure(uint16_t val){
       _min_pressure=val;
};
void  pressure_sensor::set_max_pressure(uint16_t val){
       _max_pressure=val;
};
uint16_t  pressure_sensor::get_min_pressure(){
       return _min_pressure;
};
uint16_t  pressure_sensor::get_max_pressure(){
       return _max_pressure;
};
