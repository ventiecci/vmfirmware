#ifndef PARAMETER_H
#define PARAMETER_H
/*
#define unsigned char uint8_t
#define short int16_t
#define unsigned short uint16_t
*/


/*@ parameter blower*/
#define MIN_BLOWER_SPEED 1000u
#define MAX_BLOWER_SPEED 1950u
#define DEFAULT_BLOWER_SPEED 1500u
#define MAX_BLOWER_CMH20  50
#define MIN_BLOWER_CMH20  0


/*@ parameter servo valve*/
#define CLOSE_SERVO_VALVE 1100u
#define OPEN_SERVO_VALVE 1800u

/*@ parameter  STATUS VENTILATOR*/

#define VM_STATUS_OFF   0
#define VM_STATUS_ON    1
#define VM_STATUS_TEST  2
#define VM_STATUS_ERROR 3

/*@ parameter  telemetry and telecontrol*/

#define TC_RESPIRATION_RATE 'R' 
#define TC_PIP              'P'
#define TC_PEEP             'E'
#define TC_FLOW_MAX         'F'




/* @Parameters of Mechanical Ventilation */

#endif