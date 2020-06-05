#include "mbed.h"
#include <telemetry.h>
/*
Serial command(USBTX, USBRX);
void Rx_interrupt();

void telemetry_command_init(char * buffer_rx){
    command.baud(9600); 
    command.attach(callback(Rx_interrupt, buffer_rx), RawSerial::RxIrq);

}

void debug_m(char *s , ... ){
    #if DEBUG
    command.printf(s);
    #endif  
}

void telemetry_sendOneData(float value){
    command.printf("%f\n",value); 
}


void telemetry_sendData(uint16_t pressureValue, 
                      uint16_t flowValue1,
                      uint16_t flowValue2,
                      uint8_t ValvePositionInhalation,
                      uint8_t ValvePositionExhalation,
                      uint8_t Valve02,
                      uint8_t ValveAir,
                      uint8_t batteryLevel,      
                      bool phase,
                      uint8_t VMstatus
                      
){
    command.printf("Pressure=%d",pressureValue); 
    command.printf("Flow1=%d",flowValue1); 
    command.printf("Flow2=%d",flowValue2); 
    command.printf("ValveI=%d",ValvePositionInhalation); 
    command.printf("ValveE=%d",ValvePositionExhalation); 
    command.printf("Valve02=%d",Valve02); 
    command.printf("ValveAir=%d",ValveAir); 
    command.printf("BattLevel=%d",batteryLevel); 
    command.printf("Phase=%d\n",phase); 
    
}
bool newcmd_detected;

// Interupt Routine to read in data from serial port
void Rx_interrupt(char* buffer_rx) {

  // TODO: FALTA HACER QUE NO SE QUEDE POR MAS DE U NSEGUNDO ESPERANDO DATO SI NO LLEGA FIN DE LINEA 
    debug_m("IRQ serial");
    command.scanf("%s",buffer_rx);

     #if DEBUG
      command.printf("SE ingreso: %s\n",buffer_rx);
    #endif 
   
    while (command.readable())
        command.getc();
    
      newcmd_detected =true;
   
    return;
}

void telecommand_rxData(){


}
*/