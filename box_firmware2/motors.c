/*
 * Motors functionality
 * Kutkov Oleg
 * November 2016
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "fp_leds.h"
#include "utils.h"
#include "motors.h"
#include "events.h"

#include "mb.h"
#include "mbport.h"

#define DEC_AXIS_PWM_PIN PB0
#define RA_AXIS_PWM_PIN PB1

#define RA_AXIS_PLUS_BLOCKING PC1
#define RA_AXIS_MINUS_BLOCKING PC2
#define DEC_AXIS_PLUS_BLOCKING PC3
#define DEC_AXIS_MINUS_BLOCKING PC4


/*
 * Eeprom addr variables
 * Default values is currently commented and written to the chip's eeprom
 * To write this values to the chip and use in the program:
 *  Uncomment values
 *  Build source: make clean && make
 *  Upload eeprom: make upload-eeprom
 *  Comment values
 *  Re-build source
 */
uint8_t EEMEM EEPROM_CURRENT_RA_SPEED; // = 190;
uint8_t EEMEM EEPROM_CURRENT_RA_ACCELERATION_FACTOR; // = 3;
uint8_t EEMEM EEPROM_CURRENT_RA_DECELERATION_FACTOR; // = 3;

uint8_t EEMEM EEPROM_CURRENT_DEC_SPEED; // = 190;
uint8_t EEMEM EEPROM_CURRENT_DEC_ACCELERATION_FACTOR; // = 3;
uint8_t EEMEM EEPROM_CURRENT_DEC_DECELERATION_FACTOR; // = 3;
/*
 *
 */

/*
 * Params variables loaded from eeprom
 */
static uint8_t CURRENT_RA_SPEED;
static uint8_t CURRENT_RA_ACCELERATION_FACTOR;
static uint8_t CURRENT_RA_DECELERATION_FACTOR;

static uint8_t CURRENT_DEC_SPEED;
static uint8_t CURRENT_DEC_ACCELERATION_FACTOR;
static uint8_t CURRENT_DEC_DECELERATION_FACTOR;
/*
 *
 */


/*
 * volatile variables access from the timer interrupt, avoid compiler optimizations
 */
volatile static uint8_t RA_PLUS_AXIS_ENABLED = 0;
volatile static uint8_t RA_MINUS_AXIS_ENABLED = 0;
volatile static uint8_t DEC_PLUS_AXIS_ENABLED = 0;
volatile static uint8_t DEC_MINUS_AXIS_ENABLED = 0;

volatile static uint8_t PULSE_MODE_RA_PLUS = 0;
volatile static uint8_t PULSE_MODE_DEC_PLUS = 0;

volatile static uint16_t PULSE_RA_TIMEOUT = 0;
volatile static uint16_t PULSE_DEC_TIMEOUT = 0;

volatile static uint16_t PULSE_RA_ACTIVATED = 0;
volatile static uint16_t PULSE_DEC_ACTIVATED = 0;
/*
 *
 */


static inline void configure_outputs()
{
	DDRB |= (1 << DEC_AXIS_PWM_PIN);
	DDRB |= (1 << RA_AXIS_PWM_PIN);

	DDRC |= (1 << RA_AXIS_PLUS_BLOCKING);
	DDRC |= (1 << RA_AXIS_MINUS_BLOCKING);
	DDRC |= (1 << DEC_AXIS_PLUS_BLOCKING);
	DDRC |= (1 << DEC_AXIS_MINUS_BLOCKING);
}

static inline void configure_timers()
{
	// load eeprom settings
	CURRENT_RA_SPEED = get_ra_max_speed();
	get_ra_acceleration_factor(&CURRENT_RA_ACCELERATION_FACTOR);
	get_ra_deceleration_factor(&CURRENT_RA_DECELERATION_FACTOR);

	CURRENT_DEC_SPEED = get_dec_max_speed();
	get_dec_acceleration_factor(&CURRENT_DEC_ACCELERATION_FACTOR);
	get_dec_deceleration_factor(&CURRENT_DEC_DECELERATION_FACTOR);

	// confgire first PWM channel (DEC)
	TCCR0 = (1 << COM01) | (1 << WGM00);
	TCCR0 |= (1 << CS01) | (1 << CS00); //(1 << CS01); //

	// initial PWM value
	OCR0 = 0;

	//
	// configure secons PWM channel (RA)
	TCCR2 = (1 << COM21) | (1 << WGM20);
	TCCR2 |= (1 << CS22); //(1 << CS21) | (1 << CS20); //(1 << CS21);//

	OCR2 = 0;

	//
	// configure timers for pulse methods
	TCCR3B = (1 << CS31) | (1 << CS30) | (1 << WGM32); // prescaler 64
	ETIMSK = (1 << OCIE3B);
	TCNT3 = 0x0;

	OCR3B = 6250;
}

void init_motors()
{
	configure_outputs();
	configure_timers();
}

/*
 * Parameters get/set
 */
inline void set_ra_max_speed(const uint8_t new_ra_max_speed)
{
	eeprom_write_byte(&EEPROM_CURRENT_RA_SPEED, new_ra_max_speed);
	CURRENT_RA_SPEED = new_ra_max_speed;
}

inline void set_dec_max_speed(const uint8_t new_dec_max_speed)
{
	eeprom_write_byte(&EEPROM_CURRENT_DEC_SPEED, new_dec_max_speed);
	CURRENT_DEC_SPEED = new_dec_max_speed;
}

uint8_t get_ra_max_speed()
{
	return eeprom_read_byte(&EEPROM_CURRENT_RA_SPEED);
}

uint8_t get_dec_max_speed()
{
	return eeprom_read_byte(&EEPROM_CURRENT_DEC_SPEED);
}

////

inline void set_ra_acceleration_factor(uint8_t factor)
{
	eeprom_write_byte(&EEPROM_CURRENT_RA_ACCELERATION_FACTOR, factor);
	CURRENT_RA_ACCELERATION_FACTOR = factor;
}

inline void set_ra_deceleration_factor(uint8_t factor)
{
	eeprom_write_byte(&EEPROM_CURRENT_RA_DECELERATION_FACTOR, factor);
	CURRENT_RA_DECELERATION_FACTOR = factor;
}

inline void set_dec_acceleration_factor(uint8_t factor)
{
	eeprom_write_byte(&EEPROM_CURRENT_DEC_ACCELERATION_FACTOR, factor);
	CURRENT_DEC_ACCELERATION_FACTOR = factor;
}

inline void set_dec_deceleration_factor(uint8_t factor)
{
	eeprom_write_byte(&EEPROM_CURRENT_DEC_DECELERATION_FACTOR, factor);
	CURRENT_DEC_DECELERATION_FACTOR = factor;
}

inline uint8_t get_ra_acceleration_factor()
{
	return eeprom_read_byte(&EEPROM_CURRENT_RA_ACCELERATION_FACTOR);
}

inline uint8_t get_ra_deceleration_factor()
{
	return eeprom_read_byte(&EEPROM_CURRENT_RA_DECELERATION_FACTOR);
}

inline uint8_t get_dec_acceleration_factor()
{
	return eeprom_read_byte(&EEPROM_CURRENT_DEC_ACCELERATION_FACTOR);
}

inline uint8_t get_dec_deceleration_factor()
{
	return eeprom_read_byte(&EEPROM_CURRENT_DEC_DECELERATION_FACTOR);
}

/*
 * Output PWM functionality
 */
void start_ra_plus_axis()
{
	if (RA_PLUS_AXIS_ENABLED || RA_MINUS_AXIS_ENABLED) {
		return;
	}

	PORTC |= (1 << RA_AXIS_PLUS_BLOCKING);

	RA_PLUS_AXIS_ENABLED = 1;

	enable_ra_plus_fp_led();

	uint8_t i;

	for (i = 0; i < CURRENT_RA_SPEED; i++) {
		OCR2++;
		delay_ms(CURRENT_RA_ACCELERATION_FACTOR);
	}
}

void stop_ra_plus_axis()
{
	if (RA_MINUS_AXIS_ENABLED) {
		return;
	}

	uint8_t i;

	for (i = 0; i < CURRENT_RA_SPEED; i++) {
		OCR2--;
		delay_ms(CURRENT_RA_DECELERATION_FACTOR);
	}

	OCR2 = 0;

	RA_PLUS_AXIS_ENABLED = 0;

	delay_ms(10);

	PORTC &= ~(1 << RA_AXIS_PLUS_BLOCKING);

	disable_ra_plus_fp_led();
}

void start_ra_minus_axis()
{
	if (RA_PLUS_AXIS_ENABLED || RA_MINUS_AXIS_ENABLED) {
		return;
	}

	PORTC |= (1 << RA_AXIS_MINUS_BLOCKING);
	RA_MINUS_AXIS_ENABLED = 1;

	enable_ra_minus_fp_led();

	uint8_t i;

	for (i = 0; i < CURRENT_RA_SPEED; i++) {
		OCR2++;
		delay_ms(CURRENT_RA_ACCELERATION_FACTOR);
	}
}

void stop_ra_minus_axis()
{
	if (RA_PLUS_AXIS_ENABLED) {
		return;
	}

	uint8_t i;

	for (i = 0; i < CURRENT_RA_SPEED; i++) {
		OCR2--;
		delay_ms(CURRENT_RA_DECELERATION_FACTOR);
	}

	OCR2 = 0;

	RA_MINUS_AXIS_ENABLED = 0;

	delay_ms(10);

	PORTC &= ~(1 << RA_AXIS_MINUS_BLOCKING);

	disable_ra_minus_fp_led();
}

void start_dec_plus_axis()
{
	if (DEC_PLUS_AXIS_ENABLED || DEC_MINUS_AXIS_ENABLED) {
		return;
	}

	PORTC |= (1 << DEC_AXIS_PLUS_BLOCKING);
	DEC_PLUS_AXIS_ENABLED = 1;

	enable_dec_plus_fp_led();

	uint8_t i;

	for (i = 0; i < CURRENT_DEC_SPEED; i++) {
		OCR0++;
		delay_ms(CURRENT_DEC_ACCELERATION_FACTOR);
	}
}

void stop_dec_plus_axis()
{
	if (DEC_MINUS_AXIS_ENABLED) {
		return;
	}

	uint8_t i;

	for (i = 0; i < CURRENT_DEC_SPEED; i++) {
		OCR0--;
		delay_ms(CURRENT_DEC_DECELERATION_FACTOR);
	}

	OCR0 = 0;

	DEC_PLUS_AXIS_ENABLED = 0;

	delay_ms(10);

	PORTC &= ~(1 << DEC_AXIS_PLUS_BLOCKING);

	disable_dec_plus_fp_led();
}

void start_dec_minus_axis()
{
	if (DEC_PLUS_AXIS_ENABLED || DEC_MINUS_AXIS_ENABLED) {
		return;
	}

	PORTC |= (1 << DEC_AXIS_MINUS_BLOCKING);

	DEC_MINUS_AXIS_ENABLED = 1;

	enable_dec_minus_fp_led();

	uint8_t i;

	for (i = 0; i < CURRENT_DEC_SPEED; i++) {
		OCR0++;
		delay_ms(CURRENT_DEC_ACCELERATION_FACTOR);
	}
}

void stop_dec_minus_axis()
{
	if (DEC_PLUS_AXIS_ENABLED) {
		return;
	}

	uint8_t i;

	for (i = 0; i < CURRENT_DEC_SPEED; i++) {
		OCR0--;
		delay_ms(CURRENT_DEC_DECELERATION_FACTOR);
	}

	OCR0 = 0;

	DEC_MINUS_AXIS_ENABLED = 0;

	delay_ms(10);

	PORTC &= ~(1 << DEC_AXIS_MINUS_BLOCKING);

	disable_dec_minus_fp_led();
}

void pulse_ra_plus(uint8_t time_sec)
{
	if (PULSE_RA_ACTIVATED || !time_sec) {
		return;
	}

	PULSE_MODE_RA_PLUS = 1;
	PULSE_RA_TIMEOUT = time_sec * 20;

	start_ra_plus_axis();

	PULSE_RA_ACTIVATED = 1;
}

void pulse_ra_minus(uint8_t time_sec)
{
	if (PULSE_RA_ACTIVATED || !time_sec) {
		return;
	}

	PULSE_MODE_RA_PLUS = 0;
	PULSE_RA_TIMEOUT = time_sec * 20;

	start_ra_minus_axis();

	PULSE_RA_ACTIVATED = 1;
}

void pulse_dec_plus(uint8_t time_sec)
{
	if (PULSE_DEC_ACTIVATED || !time_sec) {
		return;
	}

	PULSE_MODE_DEC_PLUS = 1;
	PULSE_DEC_TIMEOUT = time_sec * 20;

	start_dec_plus_axis();

	PULSE_DEC_ACTIVATED = 1;
}

void pulse_dec_minus(uint8_t time_sec)
{
	if (PULSE_DEC_ACTIVATED || !time_sec) {
		return;
	}

	PULSE_MODE_DEC_PLUS = 0;
	PULSE_DEC_TIMEOUT = time_sec * 20;

	start_dec_minus_axis();

	PULSE_DEC_ACTIVATED = 1;
}

uint8_t motors_busy()
{
	return (RA_PLUS_AXIS_ENABLED || RA_MINUS_AXIS_ENABLED || DEC_PLUS_AXIS_ENABLED || DEC_MINUS_AXIS_ENABLED);
}

ISR (TIMER3_COMPB_vect)
{
	if (PULSE_RA_ACTIVATED) {
		if (PULSE_RA_TIMEOUT > 0) {
			PULSE_RA_TIMEOUT--;
		} else {
			PULSE_RA_ACTIVATED = 0;

			if (PULSE_MODE_RA_PLUS) {
				post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_STOP_AXIS_PLUS, 0);
			} else {
				post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_STOP_AXIS_MINUS, 0); 
			}
		}
	}

	if (PULSE_DEC_ACTIVATED) {
		if (PULSE_DEC_TIMEOUT > 0) {
			PULSE_DEC_TIMEOUT--;
		} else {
			PULSE_DEC_ACTIVATED = 0;

			if (PULSE_MODE_DEC_PLUS) {
				post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_STOP_AXIS_PLUS, 0);
			} else {
				post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_STOP_AXIS_MINUS, 0);
			}
		}
	}

	TCNT3 = 0;
}

