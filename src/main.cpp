#include "mbed.h"

#include <blower.h>
#include <telemetry_telecontrol.h>
#include <flow_sensor.h>
#include <pressure_sensor.h>
#include <servo_valve.h>
#include <buzzer.h>
#include <o2_valve.h>
#include <Rtc-Ds1307.h>

buzzer_ctrl   buzzer(PB_7); 

DigitalIn LineAlarm(PC_3);
DigitalIn BattAlarm(PC_2);

pressure_sensor pre_sensor(PC_0);
flow_sensor flow_sensor_inh(PB_9, PB_8);
flow_sensor flow_sensor_exh(PB_3, PB_10);

o2_valve_ctrl o2_valve(PC_6);

Servo_valve svalve_inh(PA_1, "SERVO INH");
Servo_valve svalve_exh(PA_15, "SERVO EXH");

Blower blower1(PB_1,"TURBINA 1");
Blower blower2(PA_11, "tURBINA 2");


telemetry_telecontrol  cmd(USBTX, USBRX);
telemetry_telecontrol::Telemetry_str telemetry = {};

Rtc_Ds1307 rtc(PC_9,PA_8);
Rtc_Ds1307::Time_rtc tm = {};
 

uint8_t VMstatus=VM_STATUS_OFF;

Timer timetest; 
Timeout timeout_inh;
Timeout timeout_exh;


unsigned int  status_blower=0; //  0: off 1: inhalaciòn, 2: exhalaciòn:
unsigned int  RR=5; // frecuencia de respiración  minuto, entre 5 y 35 por minuto
unsigned int  PIP=40;  // Maxima presión en Cm
unsigned int  PEEP=0;   // minima presión en Cm
unsigned int  RIR; // relación de inhalación-exhalación , para covid 1 a 2  a 1 a 3,2
unsigned int  Fio2=21; // %  de fi02

unsigned int testangle=1;
float tp= 60/RR; //s
float t_inh = tp/(3); //s
float t_exh = 2*t_inh; //s
float t_iesleep = 0.1; //s


unsigned int test_pulsewidth_us =1000;

/*fuction*/
void delay( int v);   
void off_cycle();
void inhalation_cycle();
void exhalation_cycle();
void inh_exh_sleep_cycle();
void controlON();
void update_telemetry();
/*end fuction*/

void delay( int v){
    wait_ms(v);
    }


void update_telemetry(){

    telemetry.flow=flow_sensor_inh.read_flow()-flow_sensor_exh.read_flow();
    telemetry.vol=flow_sensor_inh.read_flow();
    telemetry.pre=pre_sensor.readCMH2O();
    telemetry.time = timetest.read();
    telemetry.BattAlarm=BattAlarm;
    telemetry.LineAlarm=LineAlarm;
}

void controlON(){

 
    update_telemetry();
    switch (status_blower){
        case BLOWER_OFF:
            inhalation_cycle();
        break;
        case BLOWER_INH:
            telemetry.PressureAlarm= pre_sensor.max_pressure_level(telemetry.pre);    
            if(telemetry.PressureAlarm){
                buzzer.beep(500,0.1);
            }
        break;
        case BLOWER_EXH:
            if( pre_sensor.min_pressure_level(telemetry.pre)){
                svalve_exh.openAngle(testangle);
             }
        break;
      } 
    cmd.sendData( "VENTIECCI",telemetry);
 
}

void off_cycle()
{
    rtc.getTime(tm);
 /*   cmd.debug_m("%02d:", tm.hour);
    cmd.debug_m("%02d:", tm.min);
    cmd.debug_m("%02d ", tm.sec);
    cmd.debug_m("off \n" );*/
    status_blower=BLOWER_OFF;
    blower1.speed(1000);
    blower2.speed(1000);
    svalve_inh.close();
    svalve_exh.close();
    cmd.sendData( "STANDBY",telemetry);


}



void inhalation_cycle(){
    status_blower=BLOWER_INH;
    svalve_exh.close();
    buzzer.beep(440,0.1);
    if (Fio2<100){
        blower1.cmH2O(pre_sensor.get_max_pressure());
        if (t_exh>1.5){ 
            blower2.stop();
        }
        svalve_inh.open();    
    }else
    {
        o2_valve.openA(0.8);
  
    }
    
    if (VMstatus==1)
        timeout_inh.attach(&inh_exh_sleep_cycle, t_inh);   // time to off
    else
        timeout_inh.attach(&off_cycle, t_inh);   // time to off

}

void inh_exh_sleep_cycle(){
    if (Fio2<100){
        if (t_exh>1.5){ 
            blower1.stop();
        }
    }else
    {
         o2_valve.openA(0.8); 
    }
         
    if (VMstatus==1)
        timeout_inh.attach(&exhalation_cycle, t_iesleep);   // time to off
    else
        timeout_inh.attach(&off_cycle, t_iesleep);   // time to off
  
}

void exhalation_cycle(){
    status_blower=BLOWER_EXH;
    svalve_exh.open();
  
    if (Fio2<100){
      blower2.cmH2O(pre_sensor.get_min_pressure());
      svalve_inh.close();
    }else{
          o2_valve.close();
    }

    if (VMstatus==VM_STATUS_ON)
        timeout_exh.attach(&inhalation_cycle, t_exh);   // time to off
    else
        timeout_exh.attach(&off_cycle, t_exh);   // time to off
}



void change_resp_rate(int vRR){
    cmd.debug_m("Valores actuales tp=%f, t_inh=%f, t_exh=%f \n", tp, t_inh,t_exh);
    if (vRR>0){
        RR=vRR;
        tp= 60/float(RR); //s
        t_inh = tp/(3); //s
        t_exh = 2*t_inh; //s
        t_iesleep=t_exh*0.1;
        t_exh=t_exh-t_iesleep;
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
            cmd.debug_m("el valor de RR era %d\n",RR);
            change_resp_rate(b);
            cmd.debug_m("el valor de RR es %d\n",RR);
            break;
          case TC_PIP:
            cmd.debug_m("el valor de PIP era %d\n",pre_sensor.get_max_pressure());
            pre_sensor.set_max_pressure(b);
            cmd.debug_m("el valor de PIP nuevo es %d\n",pre_sensor.get_max_pressure());
            break;
          case TC_PEEP:
            cmd.debug_m("el valor de PEEP era %d\n", pre_sensor.get_min_pressure());
            pre_sensor.set_min_pressure(b);
            cmd.debug_m("el valor de PEEP es %d\n", pre_sensor.get_min_pressure());
      
            break;
          case TC_FLOW_MAX:
            cmd.debug_m("el valor deL FLUJO es %d\n",b);
            break;
          case TC_VM_STATUS:
            VMstatus=b;
            cmd.debug_m("el valor deL VMstatus es %d\n",b);
            break;
          case TC_ANGLE_EXH:
            testangle=b;
            cmd.debug_m("el valor deL testangle es %d\n",b);
            break;
          case TC_FiO2:
            o2_valve.openA(((float)b)/100);
//            o2_valve.openD(b);
            cmd.debug_m("el valor deL fIO2 es %d\n",b);
            break;
          case TC_DUTTY:
            test_pulsewidth_us =b;
            cmd.debug_m("el valor del pulso para pwm es de  %d us\n",b);
        break;  
          default:
            cmd.debug_m("NO SE RECONOE el comando  %c con el valor %d\n",*scmd, b);
            break;
      }
       scmd = strtok(0, ",");
  } 
    return 0;          
}


void  test_blower(Blower* bw){
    svalve_exh.close();
    svalve_inh.open();
 
    bw->speed(test_pulsewidth_us);

    update_telemetry();
    cmd.sendData(bw->getname(), telemetry);

}


void   servo_valve_test(Servo_valve* svalve){
    blower1.speed(test_pulsewidth_us);
    blower2.speed(test_pulsewidth_us);
    svalve->openAngle(testangle);
    update_telemetry();
    cmd.sendData(svalve->getname(), telemetry);
  
}


void   elect_valveO2_test(o2_valve_ctrl* ovalve){
    blower1.stop();
    blower2.stop();
    ovalve->openD(test_pulsewidth_us);
    update_telemetry();
    cmd.sendData("TEST_O2_ELECTRO", telemetry);
  
}

int  main()
{

    VMstatus=VM_STATUS_OFF;
    flow_sensor_inh.init();  
    flow_sensor_exh.init();  
    o2_valve.close();
    LineAlarm.mode(PullUp);
    BattAlarm.mode(PullUp);
 //   tm.hour=2;
 //   tm.min=30;
 //   tm.sec=0;
 //   rtc.setTime(tm, true, true);

    cmd.debug_m("sistema iniciado test en modo debug" );
    wait(2.0);
    timetest.start();
    float time_act=timetest.read_ms();
    while(1) {
        if (cmd.newcmd_detected){  
           changeParameter();
           cmd.newcmd_detected=false;
        }
        if (timetest.read_ms()-time_act>=200) {
            time_act=timetest.read_ms();
            update_telemetry();
            switch(VMstatus){
            case VM_STATUS_ON:
                controlON();
                break;
            case VM_STATUS_ERROR:
                break;
            case VM_STATUS_OFF:
                off_cycle();
            break;
            case VM_TEST_BLOWER1:
                blower2.speed(1000);
                test_blower(&blower1);
            break;
            case VM_TEST_BLOWER2:
                blower1.speed(1000);
                test_blower(&blower2);
            break;
            case VM_SERVO_VALVE_TEST_INH:
                svalve_exh.close();
                servo_valve_test(&svalve_inh);
            break;
            case VM_SERVO_VALVE_TEST_EXH:
                svalve_inh.open();
                servo_valve_test(&svalve_exh);
            break;

            case VM_ELECTRO_VALVE_O2_TEST:
                svalve_inh.open();
                elect_valveO2_test(&o2_valve);
            break;
            
            default:
                off_cycle();
  
            
            }   
        }
    }  
        
}
