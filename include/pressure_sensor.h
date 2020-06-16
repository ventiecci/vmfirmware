#ifndef FLOW_SENSOR_H
#define FLOW_SEMSOR_H
#include "mbed.h"
//
//#include "arduino.h"
#include <parameter.h>
#include <telemetry.h>



class pressure_sensor{
public:
    pressure_sensor (const PinName an);
    float readRAW(void);
    float readPsi(void);
    float readCMH2O(void);
    bool min_pressure_level(float val);
    bool max_pressure_level(float val);
    void set_min_pressure(uint16_t val);
    void set_max_pressure(uint16_t val);
    uint16_t get_min_pressure();
    uint16_t get_max_pressure();

    
private:
    AnalogIn   pre_sensor;
    uint16_t  _max_pressure=40;  //cmH20 PIP
    uint16_t  _min_pressure=0;  // cmH2O PEEP
  };

#endif