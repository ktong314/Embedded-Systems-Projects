/**
 * @file pid_controller.h
 * @author Jared McArthur
 * @brief Parser for the PID python interface.
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>

typedef struct PIDController {
    uint32_t kpn;   // numerator of proportional coefficient
    uint32_t kpd;   // denominator of proportional coefficient
    uint32_t kin;   // numerator of integral coefficient
    uint32_t kid;   // denominator of integral coefficient
    uint32_t kdn;   // numerator of differential coefficient
    uint32_t kdd;   // denominator of differential coefficient
		uint32_t actual_speed;
		uint32_t desired_speed;
		uint32_t current_error;
		uint32_t total_error;
    // TODO: add additional variables (eg. error, duty cycle, etc.)
} pid_controller_t;

/**
 * @brief Initialize a pid_controller object
 * 
 * @param kpn numerator of proportional coefficient
 * @param kpd denominator of proportional coefficient
 * @param kin numerator of integral coefficient
 * @param kid denominator of integral coefficient
 * @param kdn numerator of differential coefficient
 * @param kdd denominator of differential coefficient
 */
pid_controller_t pid_controller_init(uint32_t kpn, uint32_t kpd, uint32_t kin, uint32_t kid, uint32_t kdn, uint32_t kdd);

/**
 * @brief Get the kpn object of a pid_controller object
 * 
 * @param controller 
 * @return uint32_t 
 */
uint32_t pid_controller_get_kpn(pid_controller_t* controller);

/**
 * @brief Get the kpd object of a pid_controller object
 * 
 * @param controller 
 * @return uint32_t 
 */
uint32_t pid_controller_get_kpd(pid_controller_t* controller);

/**
 * @brief Get the kin object of a pid_controller object
 * 
 * @param controller 
 * @return uint32_t 
 */
uint32_t pid_controller_get_kin(pid_controller_t* controller);

/**
 * @brief Get the kid object of a pid_controller object
 * 
 * @param controller 
 * @return uint32_t 
 */
uint32_t pid_controller_get_kid(pid_controller_t* controller);

/**
 * @brief Get the kdn object of a pid_controller object
 * 
 * @param controller 
 * @return uint32_t 
 */
uint32_t pid_controller_get_kdn(pid_controller_t* controller);

/**
 * @brief Get the kdd object of a pid_controller object
 * 
 * @param controller 
 * @return uint32_t 
 */
uint32_t pid_controller_get_kdd(pid_controller_t* controller);

// TODO: add additional functions for changing duty cycle, error, etc.