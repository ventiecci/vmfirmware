#ifndef TELEMETRY_TELECONTROL_H
#define TELEMETRY_TELECONTROL_H

//#define DEBUG 1


#include "mbed.h"
 
class telemetry_telecontrol {
 
public:

   typedef struct {
        float pre;        
        float flow;       
        float vol;       
        int PressureAlarm;     
        int LineAlarm;     
        int BattAlarm;     
        float time;       
    } Telemetry_str;

    telemetry_telecontrol(PinName pinTX,PinName pinRX);

    void debug_m(char *s , ... );
    void sendOneData(float value);
    void sendData(char *s,Telemetry_str& dat);
    
    char buffer_rx[200];
    bool newcmd_detected;
 
private :
    Serial command;
    void _Rx_interrupt();
    char  _sp_buffer;
};
 

#endif