/*
 * Events manager
 * Kutkov Oleg
 * December 2016
 */

#include "events.h"
#include "motors.h"

volatile static uint8_t RA_EVENT = EVENT_TYPE_NO_EVENT;
volatile static uint8_t DEC_EVENT = EVENT_TYPE_NO_EVENT;
volatile static uint8_t RA_PULSE_COOKIE = 0;
volatile static uint8_t DEC_PULSE_COOKIE = 0;

void events_poll()
{
	if (RA_EVENT != EVENT_TYPE_NO_EVENT) {
		switch (RA_EVENT) {
			case EVENT_TYPE_START_AXIS_PLUS:
				start_ra_plus_axis();
				break;

			case EVENT_TYPE_STOP_AXIS_PLUS:
				stop_ra_plus_axis();
				break;

			case EVENT_TYPE_START_AXIS_MINUS:
				start_ra_minus_axis();
				break;

			case EVENT_TYPE_STOP_AXIS_MINUS:
				stop_ra_minus_axis();
				break;

			case EVENT_TYPE_PULSE_AXIS_PLUS:
				pulse_ra_plus(RA_PULSE_COOKIE);
				break;

			case EVENT_TYPE_PULSE_AXIS_MINUS:
				pulse_ra_minus(RA_PULSE_COOKIE);
				break;

			default:
				break;
		}

		RA_EVENT = EVENT_TYPE_NO_EVENT;
	}

	if (DEC_EVENT != EVENT_TYPE_NO_EVENT) {
		switch (DEC_EVENT) {
			case EVENT_TYPE_START_AXIS_PLUS:
				start_dec_plus_axis();
				break;

			case EVENT_TYPE_STOP_AXIS_PLUS:
				stop_dec_plus_axis();
				break;

			case EVENT_TYPE_START_AXIS_MINUS:
				start_dec_minus_axis();
				break;

			case EVENT_TYPE_STOP_AXIS_MINUS:
				stop_dec_minus_axis();
				break;

			case EVENT_TYPE_PULSE_AXIS_PLUS:
				pulse_dec_plus(DEC_PULSE_COOKIE);
				break;

			case EVENT_TYPE_PULSE_AXIS_MINUS:
				pulse_dec_minus(DEC_PULSE_COOKIE);
				break;

			default:
				break;
		}

		DEC_EVENT = EVENT_TYPE_NO_EVENT; 
	}
}

void post_event(uint8_t evfor, uint8_t evtype, uint8_t cookie)
{
	if (evfor == EVENT_FOR_RA_AXIS) {
		RA_EVENT = evtype;
		RA_PULSE_COOKIE = cookie;
	} else if (evfor == EVENT_FOR_DEC_AXIS) {
		DEC_EVENT = evtype;
		DEC_PULSE_COOKIE = cookie;
	}
}

void post_event_lp(uint8_t evfor, uint8_t evtype, uint8_t cookie, uint8_t man_mode_touched)
{
	if (!man_mode_touched && motors_busy()) {
		return;
	}

	post_event(evfor, evtype, cookie);
}

