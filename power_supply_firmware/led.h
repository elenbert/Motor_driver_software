#ifndef LED_H
#define LED_H

void init_leds();
void enable_red_led();
void disable_red_led();
void enable_green_led();
void disable_green_led();
void enable_yellow_led();
void disable_yellow_led();
void disable_all_leds();

void leds_hello_blink();
void leds_rgy_blink();
void leds_rg_blink();
void leds_r_blink();
void leds_g_blink();
void leds_y_blink();

#endif
