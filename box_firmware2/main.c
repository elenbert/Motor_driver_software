/*
 * Main firmware entry
 * Kutkov Oleg
 * November 2016
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "utils.h"
#include "motors.h"
#include "fp_buttons.h"
#include "fp_leds.h"
#include "events.h"
#include "ps_controller.h"

#include "mb.h"
#include "mbport.h"
#include "mbrtu.h"

#define MODBUS_ENABLE_SW PC0
#define MODBUS_ADDR_1_SW PD6
#define MODBUS_ADDR_2_SW PD5
#define MODBUS_ADDR_3_SW PD4
#define MODBUS_ADDR_4_SW PD3

/*
 * Modbus specific
 */
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4

static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];

volatile static uint8_t modbus_enabled = 0;
/*
*/

static inline void init_mb_switches()
{
	/* Configure MODBUS switches as INPUT */
	DDRC &= ~(1 << MODBUS_ENABLE_SW);
	DDRD &= ~(1 << MODBUS_ADDR_1_SW);
	DDRD &= ~(1 << MODBUS_ADDR_2_SW);
	DDRD &= ~(1 << MODBUS_ADDR_3_SW);
	DDRD &= ~(1 << MODBUS_ADDR_4_SW);

	/* Enable internal pull-up resistors for each MODBUS switch */
	PORTC |= (1 << MODBUS_ENABLE_SW);
	PORTD |= (1 << MODBUS_ADDR_1_SW);
	PORTD |= (1 << MODBUS_ADDR_2_SW);
	PORTD |= (1 << MODBUS_ADDR_3_SW);
	PORTD |= (1 << MODBUS_ADDR_4_SW);
}
/* 
 */

/*
 * Modbus switches
 */
static inline uint8_t is_modubus_mode_enabled()
{
	return !(PINC & (1 << MODBUS_ENABLE_SW));
}
/*
 */


/*
 * Main program entry
 */
int main()
{
	init_mb_switches();

	modbus_enabled = is_modubus_mode_enabled();

	init_fp_leds();

	init_motors();

	if (!modbus_enabled) {
		xMBPortSerialInit(9600);
		vMBPortSerialEnable(TRUE, FALSE);
	} else {
		eMBInit(MB_RTU, 0x0A, 0, 9600, MB_PAR_NONE);
		eMBEnable();	
	}

	if (!modbus_enabled) {
		vMBPortSerialEnable(FALSE, TRUE);
		xMBPortSerialPutByte(0x2A);
		vMBPortSerialEnable(TRUE, FALSE);
	}

	sei();

/*	init_ps_controller();

	do {
		init_ps();
		ps_poll();
		hello_fp_led_blink();
	} while (!is_ps_activated());

	do {
		enable_ps();
		ps_poll();
		hello_fp_led_blink();
	} while (!is_ps_enabled());
*/
	init_fp_buttons();

	while(1) {
		eMBPoll();
		events_poll();
		fp_buttons_poll();

//		ps_ping();
//		ps_poll();

		/* lost connection to ps, halt the system */
//		if (!is_ps_online()) {	
//			cli();

//			while (1) {
//				hello_fp_led_blink();
//			}
//		}
	}
}

/*
 * Main (link to PC) USART interrupt handlder with data processing
 */
ISR(USART1_RXC_vect)
{
	if (modbus_enabled) {
		xMBRTUReceiveFSM();
		return;
	}

	/* Old (Sergeev) protocol implementation */

	UCHAR timeout = 0;
	UCHAR rxd;
	xMBPortSerialGetByte(&rxd);

	switch (rxd) {
		case 0xF2:
			post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_START_AXIS_PLUS, 0);
			break;

		case 0xF3:
			post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_STOP_AXIS_PLUS, 0);
			break;

		case 0xF4:
			post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_START_AXIS_MINUS, 0);
			break;

		case 0xF5:
			post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_STOP_AXIS_MINUS, 0);
			break;

		case 0xF8:
			post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_START_AXIS_PLUS, 0);
			break;

		case 0xF9:
			post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_STOP_AXIS_PLUS, 0);
			break;

		case 0xFA:
			post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_START_AXIS_MINUS, 0);
			break;

		case 0xFB:
			post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_STOP_AXIS_MINUS, 0);
			break;

		case 0xC2:
			xMBPortSerialGetByteWait(&timeout);
			post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_PULSE_AXIS_PLUS, timeout);
			break;

		case 0xC3:
			xMBPortSerialGetByteWait(&timeout);
			post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_PULSE_AXIS_MINUS, timeout);
			break;

		case 0xC4:
			xMBPortSerialGetByteWait(&timeout);
			post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_PULSE_AXIS_PLUS, timeout);
			break;

		case 0xC5:
			xMBPortSerialGetByteWait(&timeout);
			post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_PULSE_AXIS_MINUS, timeout);
			break;

		default:
			vMBPortSerialEnable(FALSE, TRUE);
			xMBPortSerialPutByte(0x1);
			vMBPortSerialEnable(TRUE, FALSE);
			break;
	}

	/*  */

	vMBPortSerialEnable(FALSE, TRUE);
	xMBPortSerialPutByte(0x0);
	vMBPortSerialEnable(TRUE, FALSE);
}


/*
 * Modbus callbacks implementation
 */
eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) ) 
    {   
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 ) 
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }   
    else
    {   
        eStatus = MB_ENOREG;
    }

    return eStatus;
}


inline static void value_to_pucbuf(UCHAR * pucRegBuffer, uint8_t val)
{
	*pucRegBuffer++ = ( unsigned char ) (val >> 8);
	*pucRegBuffer++ = ( unsigned char ) (val & 0xFF);
}

inline static uint8_t pucbuf_to_value(UCHAR * pucRegBuffer)
{
	uint8_t val = *pucRegBuffer++ << 8;
	val |= *pucRegBuffer++;

	return val;
}

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress
								, USHORT usNRegs, eMBRegisterMode eMode)
{
	if (usNRegs != 1) {
		return MB_ENOREG;
	}

	eMBErrorCode eStatus = MB_ENOERR;

	if (eMode == MB_REG_WRITE) {
		switch (usAddress - 1) {
			/* Set motion settings */

			case 0: // set new RA acceleration factor
				set_ra_acceleration_factor(pucbuf_to_value(pucRegBuffer));
				break;

			case 1: // set new RA deceleration factor
				set_ra_deceleration_factor(pucbuf_to_value(pucRegBuffer));
				break;

			case 2: // set new DEC acceleration factor
				set_dec_acceleration_factor(pucbuf_to_value(pucRegBuffer));
				break;

			case 3: // set new DEC deceleration factor
				set_dec_deceleration_factor(pucbuf_to_value(pucRegBuffer));
				break;

			case 4: // set new RA max speed
				set_ra_max_speed(pucbuf_to_value(pucRegBuffer));
				break;

			case 5: // set new DEC max speed
				set_dec_max_speed(pucbuf_to_value(pucRegBuffer));
				break;
			/* */

			/* Axis control */

			case 10: // RA axis start/stop wo timeout
				// reg data contains event type, see events.h
				post_event(EVENT_FOR_RA_AXIS, pucbuf_to_value(pucRegBuffer), 0);
				break;

			case 11: // RA plus axis PULSE with timeout
				// reg data contains required timeout
				post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_PULSE_AXIS_PLUS, pucbuf_to_value(pucRegBuffer));
				break;

			case 12: // RA minus axis PULSE with timeout
				post_event(EVENT_FOR_RA_AXIS, EVENT_TYPE_PULSE_AXIS_MINUS, pucbuf_to_value(pucRegBuffer));
				break;

			case 20: // DEC axis start/stop wo timeout
				post_event(EVENT_FOR_DEC_AXIS, pucbuf_to_value(pucRegBuffer), 0);
				break;

			case 21: // DEC plus axis PULSE with timeout
				post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_PULSE_AXIS_PLUS, pucbuf_to_value(pucRegBuffer));
				break;

			case 22: // DEC minus axis PULSE with timeout
				post_event(EVENT_FOR_DEC_AXIS, EVENT_TYPE_PULSE_AXIS_MINUS, pucbuf_to_value(pucRegBuffer));
				break;

			/* */

			default:
				eStatus = MB_ENOREG;
				break;
		}
	} else {
		switch (usAddress - 1) {
			/* Get motion settings */ 

			case 0: // Get RA acceleration factor
				value_to_pucbuf(pucRegBuffer, get_ra_acceleration_factor());
				break;

			case 1: // Get RA deceleration factor
				value_to_pucbuf(pucRegBuffer, get_ra_deceleration_factor());
				break;

			case 2: // Get DEC acceleration factor
				value_to_pucbuf(pucRegBuffer, get_dec_acceleration_factor());
				break;

			case 3: // Get DEC deceleration factor
				value_to_pucbuf(pucRegBuffer, get_dec_deceleration_factor());
				break;

			case 4: // Get RA max speed
				value_to_pucbuf(pucRegBuffer, get_ra_max_speed());
				break;

			case 5: // Get DEC max speed
				value_to_pucbuf(pucRegBuffer, get_dec_max_speed());
				break;

			case 70: // Initial hw connection test
				value_to_pucbuf(pucRegBuffer, 100); // just answer with 100 value
				break;

			/* */

			default:
				eStatus = MB_ENOREG;
				break;
		}
	}

	return eStatus;
}


