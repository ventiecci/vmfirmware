#include "mbed.h"
#include <telemetry_telecontrol.h>

telemetry_telecontrol::telemetry_telecontrol(PinName pinTX,PinName pinRX):command(pinTX, pinRX) {
    command.baud(9600); 
    command.attach(this,&telemetry_telecontrol::_Rx_interrupt, RawSerial::RxIrq);
    newcmd_detected=false;
    _sp_buffer=0;

};

// Interupt Routine to read in data from serial port
void telemetry_telecontrol::_Rx_interrupt() {

    while (command.readable()){
        char  byteIn =command.getc();
        if(newcmd_detected==false)
        if (byteIn==0x0D || byteIn ==0x0A){
            buffer_rx[_sp_buffer]=0;
            newcmd_detected=true;
        }
        else{
            buffer_rx[_sp_buffer]=byteIn;
            _sp_buffer++;
        }
    }
    if(newcmd_detected==true){
        #if DEBUG
           for (int i =0; i<_sp_buffer;i++) 
               command.putc(buffer_rx[i]);
        #endif
        _sp_buffer=0;   
    }
        
    return;
};

void telemetry_telecontrol::debug_m(char *s , ... ){
    #if DEBUG
        command.printf(s);
    #endif  
};


void telemetry_telecontrol::sendOneData(float value){
    command.printf("%f",value); 
}

void telemetry_telecontrol::sendData(char *s, Telemetry_str& dat){
    command.printf(s);
    command.printf("\t%0.4f", dat.pre); 
    command.printf("\t%0.4f", dat.flow);       
    command.printf("\t%0.4f", dat.vol);       
    command.printf("\t%d", dat.PressureAlarm);     
    command.printf("\t%d", dat.LineAlarm);     
    command.printf("\t%d", dat.BattAlarm);     
    command.printf("\t%0.4f", dat.time);     
    command.printf("\n");
   
}
