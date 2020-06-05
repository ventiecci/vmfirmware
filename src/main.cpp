#include "mbed.h"

#include <blower.h>
#include <telemetry_telecontrol.h>
#include <flow_sensor.h>
#include <pressure_sensor.h>
#include <servo_valve.h>
#include "buzzer.h"

buzzer_ctrl   buzzer(PB_7); 

Servo_valve svalve_inh(PA_15);
Servo_valve svalve_exh(PA_0);
pressure_sensor pre_sensor(PC_0);


Blower blower1(PB_1);
Blower blower2(PA_11);
flow_sensor flow_sensor_inh(PB_9, PB_8);
flow_sensor flow_sensor_enh(PB_3, PB_10);

telemetry_telecontrol  cmd(USBTX, USBRX);


uint8_t VMstatus=VM_STATUS_OFF;

Timer timetest; 
Timeout timeout_inh;
Timeout timeout_exh;



unsigned int  RR=5; // frecuencia de respiración  minuto, entre 5 y 35 por minuto
unsigned int  PIP=40;  // Maxima presión en Cm
unsigned int  PEEP=0;   // minima presión en Cm
unsigned int  RIR; // relación de inhalación-exhalación , para covid 1 a 2  a 1 a 3,2

float tp= 60/RR; //s
float t_inh = tp/(3); //s
float t_exh = 2*t_inh; //s

/*fuction*/
void delay( int v);
void off_cycle();
void inhalation_cycle();
void exhalation_cycle();

/*end fuction*/

void delay( int v){
    wait_ms(v);
    }



void off_cycle()
{
    cmd.debug_m("sistema off\n" );
    blower1.speed(1000);
    blower2.speed(1000);
    svalve_inh.close();
    svalve_exh.close();

}
int delta_p=0;
void inhalation_cycle(){
//    cmd.debug_m("sistema inhalación\n" );
    blower1.speed(1500);
   // blower2.speed(1500);
    blower2.cmH2O(pre_sensor.get_max_pressure()-delta_p);
    svalve_inh.open();
    svalve_exh.close();
    buzzer.beep(440,0.1);
  
    if (VMstatus==1)
        timeout_inh.attach(&exhalation_cycle, t_inh);   // time to off
    else
        timeout_inh.attach(&off_cycle, t_inh);   // time to off


}

void exhalation_cycle(){
 //   cmd.debug_m("sistema exhalación\n" );
  
    if (t_exh>1.5){ 
        blower1.stop();
        blower2.stop();
    }
    svalve_inh.close();
   // wait_ms(100);
    svalve_exh.open();
    if (VMstatus==1)
        timeout_exh.attach(&inhalation_cycle, t_exh);   // time to off
    else
        timeout_exh.attach(&off_cycle, t_exh);   // time to off
}

void   servo_valve_test(Servo_valve* svalve){
    timetest.start();     // start timer counting 
    blower1.speed(1500);
    blower2.speed(1500);
    bool onm=0;
    while(VMstatus==VM_STATUS_TEST){
       cmd.sendOneData( flow_sensor_inh.read_flow());
        if (timetest.read_ms()>=2000) 
        { 
            timetest.reset(); 
            if(onm==1) {
                cmd.debug_m("m2\n" );
                svalve->close();
                onm=0;
            
            }else {
                svalve->open();
                cmd.debug_m("m3\n" );
                onm=1;
            }
        }


}
}


void change_resp_rate(int vRR){
    cmd.debug_m("Valores actuales tp=%f, t_inh=%f, t_exh=%f \n", tp, t_inh,t_exh);
    if (vRR>0){
        RR=vRR;
        tp= 60/float(RR); //s
        t_inh = tp/(3); //s
        t_exh = 2*t_inh; //s
    }
    cmd.debug_m("Nuevo valor para tp=%f, t_inh=%0.4f, t_exh=%f \n", tp, t_inh,t_exh);
}

int  changeParameter(){
    char *scmd = strtok(cmd.buffer_rx, ",");
    while (scmd != 0){
        char *sep = strchr(scmd, '=');
        *sep = 0;
        int a = atoi(scmd);
        ++sep;
        int b = atoi(sep);
        switch (*scmd){
          case 'T':
            cmd.debug_m("el valor de T es %d\n",b);
            break;
          case TC_RESPIRATION_RATE:
            cmd.debug_m("el valor de RR es %d\n",b);
            change_resp_rate(b);
            break;
          case TC_PIP:
            cmd.debug_m("el valor de PIP es %d\n",b);
            pre_sensor.set_max_pressure(b);
            cmd.debug_m("el valor de PIP nuevo es %d\n",pre_sensor.get_max_pressure());
            break;
          case TC_PEEP:
            cmd.debug_m("el valor de PEEP es %d\n",b);
            pre_sensor.set_min_pressure(b);
            cmd.debug_m("el valor de PEEP es %d\n", pre_sensor.get_min_pressure());
      
            break;
          case TC_FLOW_MAX:
            cmd.debug_m("el valor deL FLUJO es %d\n",b);
            break;
          default:
            cmd.debug_m("NO SE RECONOE el comando  %c con el valor %d\n",*scmd, b);
            break;
      }
       scmd = strtok(0, ",");
  } 
    return 0;          
}


int  main()
{

    VMstatus=VM_STATUS_OFF;
   
    cmd.debug_m("sistema iniciado test en modo debug" );
    wait(5.0);
   
    flow_sensor_inh.init();
    
    VMstatus=VM_STATUS_TEST;
    VMstatus=VM_STATUS_ON;
    exhalation_cycle();
    float flow_act;
    float  presion_act;
    while(1) {
        if (cmd.newcmd_detected){  
           cmd.newcmd_detected=false;
           changeParameter();
        }
        switch(VMstatus){
        case VM_STATUS_ON:
           flow_act=flow_sensor_inh.read_flow();
           presion_act=pre_sensor.readCMH2O();
           cmd.debug_m("%0.6f \t", flow_act );
           cmd.debug_m("%0.6f \t",presion_act);
           cmd.sendOneData(pre_sensor.get_max_pressure());
           cmd.debug_m("\n");
           
            if( pre_sensor.max_pressure_level(presion_act)){
                      buzzer.beep(500,0.1);
     //                 delta_p=delta_p+1;

             }
 
        //  cmd.sendOneData( flow_act);
        //  cmd.sendOneData( presion_act);
        
            delay(100);

            break;
        case VM_STATUS_TEST:
            servo_valve_test(&svalve_inh);
            break;
        case VM_STATUS_ERROR:
            break;
        case VM_STATUS_OFF:
            off_cycle();
        break;
        default:
            off_cycle();
        
        }

    }  
        
}
