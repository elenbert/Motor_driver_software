
#include <avr/io.h>
#include "utils.h"
#include "events.h"

#define RA_PLUS_BUTTON_PIN PA0
#define RA_MINUS_BUTTON_PIN PA1
#define DEC_PLUS_BUTTON_PIN PA2
#define DEC_MINUS_BUTTON_PIN PA3

static uint8_t ra_plus_bt_enabled = 0;
static uint8_t ra_minus_bt_enabled = 0;
static uint8_t dec_plus_bt_enabled = 0;
static uint8_t dec_minus_bt_enabled = 0;

void init_fp_buttons()
{
	/* Configure front panel buttons pins as INPUT */
	DDRA &= ~(1 << RA_PLUS_BUTTON_PIN);
	DDRA &= ~(1 << RA_MINUS_BUTTON_PIN);
	DDRA &= ~(1 << DEC_PLUS_BUTTON_PIN);
	DDRA &= ~(1 << DEC_MINUS_BUTTON_PIN);

	/* Enable internal pull-up resistors for each button pins  */
	PORTA |= (1 << RA_PLUS_BUTTON_PIN);
	PORTA |= (1 << RA_MINUS_BUTTON_PIN);
	PORTA |= (1 << DEC_PLUS_BUTTON_PIN);
	PORTA |= (1 << DEC_MINUS_BUTTON_PIN);
}

/*
 * Front panel buttons functionality
 */
static inline uint8_t is_ra_plus_button_unpressed()
{
	return PINA & (1 << RA_PLUS_BUTTON_PIN);
}

static inline uint8_t is_ra_plus_button_pressed()
{
	return !is_ra_plus_button_unpressed();
}

static inline uint8_t is_ra_minus_button_unpressed()
{
	return PINA & (1 << RA_MINUS_BUTTON_PIN);
}

static inline uint8_t is_ra_minus_button_pressed()
{
	return !is_ra_minus_button_unpressed();
}

static inline uint8_t is_dec_plus_button_unpressed()
{
	return PINA & (1 << DEC_PLUS_BUTTON_PIN);
}

static inline uint8_t is_dec_plus_button_pressed()
{
	return !is_dec_plus_button_unpressed();
}

static inline uint8_t is_dec_minus_button_unpressed()
{
	return PINA & (1 << DEC_MINUS_BUTTON_PIN);
}

static inline uint8_t is_dec_minus_button_pressed()
{
	return !is_dec_minus_button_unpressed();
}

void fp_buttons_poll()
{
	uint8_t man_mode = (ra_plus_bt_enabled || ra_minus_bt_enabled || dec_plus_bt_enabled || dec_minus_bt_enabled);

	if (is_ra_plus_button_pressed()) {
		delay_ms(500);

		if (is_ra_plus_button_pressed()) {
			if (!ra_minus_bt_enabled) {
				post_event_lp(EVENT_FOR_RA_AXIS, EVENT_TYPE_START_AXIS_PLUS, 0, man_mode);
				ra_plus_bt_enabled = 1;
			}
		} 
	} else if (is_ra_plus_button_unpressed()) {
		if (ra_plus_bt_enabled) {
			post_event_lp(EVENT_FOR_RA_AXIS, EVENT_TYPE_STOP_AXIS_PLUS, 0, man_mode);
			ra_plus_bt_enabled = 0;
		}
	}

	if (is_ra_minus_button_pressed()) {
		delay_ms(500);

		if (is_ra_minus_button_pressed()) {
			if (!ra_plus_bt_enabled) {
				post_event_lp(EVENT_FOR_RA_AXIS, EVENT_TYPE_START_AXIS_MINUS, 0, man_mode);
				ra_minus_bt_enabled = 1;
			}
		}
	} else if (is_ra_minus_button_unpressed()) {
		if (ra_minus_bt_enabled) {
			post_event_lp(EVENT_FOR_RA_AXIS, EVENT_TYPE_STOP_AXIS_MINUS, 0, man_mode);
			ra_minus_bt_enabled = 0;
		}
	}

	if (is_dec_plus_button_pressed()) {
		delay_ms(500);

		if (is_dec_plus_button_pressed()) {
			if (!dec_minus_bt_enabled) {
				post_event_lp(EVENT_FOR_DEC_AXIS, EVENT_TYPE_START_AXIS_PLUS, 0, man_mode);
				dec_plus_bt_enabled = 1;
			}
		}
	} else if (is_dec_plus_button_unpressed()) {
		if (dec_plus_bt_enabled) {
			post_event_lp(EVENT_FOR_DEC_AXIS, EVENT_TYPE_STOP_AXIS_PLUS, 0, man_mode);
			dec_plus_bt_enabled = 0;
		}
	}

	if (is_dec_minus_button_pressed()) {
		delay_ms(500);

		if (is_dec_minus_button_pressed()) {
			if (!dec_plus_bt_enabled) {
				post_event_lp(EVENT_FOR_DEC_AXIS, EVENT_TYPE_START_AXIS_MINUS, 0, man_mode);
				dec_minus_bt_enabled = 1;
			}
		}
	} else if (is_dec_minus_button_unpressed()) {
		if (dec_minus_bt_enabled) {
			post_event_lp(EVENT_FOR_DEC_AXIS, EVENT_TYPE_STOP_AXIS_MINUS, 0, man_mode);
			dec_minus_bt_enabled = 0;
		}
	}
}

