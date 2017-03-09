/*
 * Header file for motors functionality
 * Kutkov Oleg
 * November 2016
 */

#ifndef __MOTORS_H__
#define __MOTORS_H__

#include <stdint.h>

void init_motors();

void set_ra_max_speed(const uint8_t new_ra_max_speed);
void set_dec_max_speed(const uint8_t new_dec_max_speed);
inline uint8_t get_ra_max_speed();
inline uint8_t get_dec_max_speed();

inline void set_ra_acceleration_factor(uint8_t factor);
inline void set_ra_deceleration_factor(uint8_t factor);
inline void set_dec_acceleration_factor(uint8_t factor);
inline void set_dec_deceleration_factor(uint8_t factor);
inline uint8_t get_ra_acceleration_factor();
inline uint8_t get_ra_deceleration_factor();
inline uint8_t get_dec_acceleration_factor();
inline uint8_t get_dec_deceleration_factor();

void start_ra_plus_axis();
void stop_ra_plus_axis();
void start_ra_minus_axis();
void stop_ra_minus_axis();
void start_dec_plus_axis();
void stop_dec_plus_axis();
void start_dec_minus_axis();
void stop_dec_minus_axis();

void pulse_ra_plus(uint8_t time_sec);
void pulse_ra_minus(uint8_t time_sec);
void pulse_dec_plus(uint8_t time_sec);
void pulse_dec_minus(uint8_t time_sec);

uint8_t motors_busy();

#endif

