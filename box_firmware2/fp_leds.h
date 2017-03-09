/*
 *
 */

#ifndef __FP_LEDS_H__
#define __FP_LEDS_H__

void init_fp_leds();

void enable_dec_minus_fp_led();
void disable_dec_minus_fp_led();
void enable_dec_plus_fp_led();
void disable_dec_plus_fp_led();
inline void enable_ra_minus_fp_led();
void disable_ra_minus_fp_led();
void enable_ra_plus_fp_led();
void disable_ra_plus_fp_led();

inline void hello_fp_led_blink();

#endif

