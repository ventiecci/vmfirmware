#ifndef FLOW_SENSOR_H
#define FLOW_SEMSOR_H
#include "mbed.h"
//#include "arduino.h"
#include <parameter.h>


#define D6F_ADDR 0x6C<<1 


class flow_sensor{
public:
    flow_sensor (const PinName sda,const PinName scl);
    float read_flow(void);
    uint8_t init(void);
    int scanI2C(void);
  
private:
    I2C sensorD6F;
    uint8_t _slave_addr;

    char d6f_config[5];
    char d6f_comp_read[4];
    char d6f_mem_read[1];
    char d6f_init[2];
};

#endif