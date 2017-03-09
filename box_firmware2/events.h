/*
 * Header file for events manager
 * Kutkov Oleg
 * December 2016
 */

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include <inttypes.h>

#define EVENT_FOR_RA_AXIS 0x11
#define EVENT_FOR_DEC_AXIS 0x12

#define EVENT_TYPE_NO_EVENT	0x00

#define EVENT_TYPE_START_AXIS_PLUS 0x20
#define EVENT_TYPE_STOP_AXIS_PLUS 0x21

#define EVENT_TYPE_START_AXIS_MINUS 0x23
#define EVENT_TYPE_STOP_AXIS_MINUS 0x24

#define EVENT_TYPE_PULSE_AXIS_PLUS 0x22
#define EVENT_TYPE_PULSE_AXIS_MINUS 0x25

void events_poll();

void post_event(uint8_t evfor, uint8_t evtype, uint8_t cookie);
void post_event_lp(uint8_t evfor, uint8_t evtype, uint8_t cookie, uint8_t man_mode_touched);

#endif

