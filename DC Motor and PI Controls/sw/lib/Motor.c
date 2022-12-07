/**
 * @file motor.c
 * @author Allen Viljoen (allenviljoen@utexas.edu)
 * @brief 
	uses timer 3
	
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "Motor.h"
#include "../inc/tm4c123gh6pm.h"
#include "/inc/PWM.h"
#include "/inc/Timer3A.h"



extern pid_controller_t pid_controller;

uint32_t tot_error; //add these to controller struct after testing
uint32_t error;
uint32_t u_integ_global;

void Motor_Init(void){
	pid_controller_t pid_controller = pid_controller_init(0, 0, 0, 0, 0, 0); //initialize pid controller
	Timer3A_Init(&update_duty_cycle, 1, 3); //should interrupt 10 times faster than motor tau
}

void update_duty(uint16_t duty){
	PWM0A_Duty(duty);
}

void update_duty_cycle(void){
	uint32_t u_prop = (pid_controller_get_kpn(&pid_controller) * error)/pid_controller_get_kpd(&pid_controller); //calculate U_prop
	tot_error += error;
	uint32_t u_integ = (pid_controller_get_kin(&pid_controller) * tot_error)/pid_controller_get_kid(&pid_controller);//calculate U_integral
	uint32_t u = u_prop + u_integ; //calculate U_total
	PWM0A_Duty(u); //update duty
}
