/**
 * @file pid_controller_parser.c
 * @author Jared McArthur
 * @brief 
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "lib/pid_controller/pid_controller.h"
#include "inc/PWM.h"

pid_controller_t pid_controller_init(uint32_t kpn, uint32_t kpd, uint32_t kin, uint32_t kid, uint32_t kdn, uint32_t kdd) {
    pid_controller_t pid_controller = {
        .kpn = kpn,
        .kpd = kpd,
        .kin = kin,
        .kid = kid,
        .kdn = kdn,
        .kdd = kdd,
		.actual_speed = 0,
		.desired_speed = 0,
		.current_error = 0,
		.total_error = 0,
    };
	
	PWM0A_Init(40000, 4000); // start at 10% duty on PB6
	
    // TODO: initialize timer(s) to change duty cycle, get error, etc.

    return pid_controller;
}

uint32_t pid_controller_get_kpn(pid_controller_t* controller) {
    return controller->kpn;
}

uint32_t pid_controller_get_kpd(pid_controller_t* controller) {
    return controller->kpd;
}

uint32_t pid_controller_get_kin(pid_controller_t* controller) {
    return controller->kin;
}

uint32_t pid_controller_get_kid(pid_controller_t* controller) {
    return controller->kid;
}

uint32_t pid_controller_get_kdn(pid_controller_t* controller) {
    return controller->kdn;
}

uint32_t pid_controller_get_kdd(pid_controller_t* controller) {
    return controller->kdd;
}

void pid_controller_set_kpn(pid_controller_t* controller, uint32_t kpn) {
    controller->kpn = kpn;
}

void pid_controller_set_kpd(pid_controller_t* controller, uint32_t kpd) {
    controller->kpd = kpd;
}

void pid_controller_set_kin(pid_controller_t* controller, uint32_t kin) {
    controller->kin = kin;
}

void pid_controller_set_kid(pid_controller_t* controller, uint32_t kid) {
    controller->kid = kid;
}

void pid_controller_set_kdn(pid_controller_t* controller, uint32_t kdn) {
    controller->kdn = kdn;
}

void pid_controller_set_kdd(pid_controller_t* controller, uint32_t kdd) {
    controller->kdd = kdd;
}


