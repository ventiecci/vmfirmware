    #include <parameter.h>
    #include <flow_sensor.h>


 flow_sensor::flow_sensor (const PinName sda, const PinName scl):sensorD6F(sda, scl){

    _slave_addr=D6F_ADDR ;
    // D6F setup: EEPROM Control <= 0x00h
 
     
 };
uint8_t flow_sensor::init(void) {

    uint8_t error;
  //  sensorD6F.frequency (200000);
    // Init config cmd
    //d6f_config[5] = {0x00, 0xD0, 0x40, 0x18, 0x06};
    d6f_config[0] = 0x00;
    d6f_config[1] = 0xD0;
    d6f_config[2] = 0x40;
    d6f_config[3] = 0x18;
    d6f_config[4] = 0x06;
    
    //d6f_comp_read[4] = {0x00, 0xD0, 0x51, 0x2C};
    d6f_comp_read[0] = 0x00;
    d6f_comp_read[1] = 0xD0;
    d6f_comp_read[2] = 0x51;
    d6f_comp_read[3] = 0x2C;
    
    d6f_mem_read[0] = 0x07;
    
    d6f_init[0] = 0x0B;
    d6f_init[1] = 0x00;
    error = sensorD6F.write(D6F_ADDR, d6f_init, 2);
    
    return(error);

};

float flow_sensor::read_flow(void) {
    char error;
    char rdata[2];
    uint16_t raw_flow;
    
    error = sensorD6F.write(_slave_addr, d6f_config, 5);
    wait_us(160);
    error |= sensorD6F.write(_slave_addr, d6f_comp_read, 4);
    error |= sensorD6F.write(_slave_addr, d6f_mem_read, 1, true);
    error |= sensorD6F.read(_slave_addr, rdata, 2);
    if(error) {
        return(error);
    }
    raw_flow = (rdata[0]<<8) + rdata[1];
    return  (((float)raw_flow - 1024.0) * 70.0 / 42000.0);   // Flow sensor D6F-70 :  0-70[L/min] range
  
};


int flow_sensor::scanI2C(void) {

  int ack;   
  int address; 
  for(address=1;address<127;address++) {   
    ack = sensorD6F.write(0x6C<<1, "11", 1);
    if (ack == 0) {
       return address;
    }    
  }
  return 0;
};