
#include <avr/io.h>
#include <util/delay.h>
#include "fp_leds.h"

#define DEC_PLUS_LED_PIN PA5
#define DEC_MINUS_LED_PIN PA4
#define RA_PLUS_LED_PIN PA7
#define RA_MINUS_LED_PIN PA6

/*
 * IO initialization
 */
void init_fp_leds()
{
	DDRA |= (1 << RA_MINUS_LED_PIN);
	DDRA |= (1 << RA_PLUS_LED_PIN);
	DDRA |= (1 << DEC_MINUS_LED_PIN);
	DDRA |= (1 << DEC_PLUS_LED_PIN);
}

/*
 * Front panel LEDS enable/disable
 */
inline void enable_dec_minus_fp_led()
{
	PORTA |= (1 << DEC_MINUS_LED_PIN);
}

inline void disable_dec_minus_fp_led()
{
	PORTA &= ~(1 << DEC_MINUS_LED_PIN);
}

inline void enable_dec_plus_fp_led()
{
	PORTA |= (1 << DEC_PLUS_LED_PIN);
}

inline void disable_dec_plus_fp_led()
{
	PORTA &= ~(1 << DEC_PLUS_LED_PIN);
}

inline void enable_ra_minus_fp_led()
{
	PORTA |= (1 << RA_MINUS_LED_PIN);
}

inline void disable_ra_minus_fp_led()
{
	PORTA &= ~(1 << RA_MINUS_LED_PIN);
}

inline void enable_ra_plus_fp_led()
{
	PORTA |= (1 << RA_PLUS_LED_PIN);
}

inline void disable_ra_plus_fp_led()
{
	PORTA &= ~(1 << RA_PLUS_LED_PIN);
}

/*
 * Startup LED blinking
 */
inline void hello_fp_led_blink()
{
	PORTA |= (1 << RA_MINUS_LED_PIN);
	PORTA |= (1 << DEC_MINUS_LED_PIN);

	_delay_ms(250);

	PORTA |= (1 << RA_PLUS_LED_PIN);
	PORTA |= (1 << DEC_PLUS_LED_PIN);

	_delay_ms(250);

	PORTA &= ~(1 << DEC_MINUS_LED_PIN);
	PORTA &= ~(1 << DEC_PLUS_LED_PIN);
	PORTA &= ~(1 << RA_MINUS_LED_PIN);
	PORTA &= ~(1 << RA_PLUS_LED_PIN);
}

