/*
 * FreeModbus Libary: ATMega162 Port
 * Copyright (C) 2016 Oleg Kutkov <kutkov.o@yandex.ru>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.6 2006/09/17 16:45:53 wolti Exp $
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "mb.h"
#include "port.h"
#include "mbrtu.h"

#include "../../fp_leds.h"
#include "../../utils.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#define UART_BAUD_RATE          9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) \
    ( ( F_OSC ) / ( ( UART_BAUD_RATE ) * 16UL ) - 1 )

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    UCSRB |= _BV( TXEN1 ) | _BV(TXCIE1);

    if( xRxEnable )
    {
        UCSRB |= _BV( RXEN1 ) | _BV( RXCIE1 );
    }
    else
    {
        UCSRB &= ~( _BV( RXEN1 ) | _BV( RXCIE1 ) );
    }

    if( xTxEnable )
    {
        UCSRB |= _BV(TXEN1) | _BV(UDRIE1);
        RTS_HIGH;
    }
    else
    {
        UCSRB &= ~( _BV( UDRIE1 ) );
    }
}

void xMBPortSerialInit(ULONG ulBaudRate)
{
	UBRR = UART_BAUD_CALC(ulBaudRate, F_CPU);

	UCSR1C = (1 << URSEL1) | (1 << UCSZ10) | ( 1 << UCSZ11);
	
    vMBPortSerialEnable( FALSE, FALSE );

    RTS_INIT;
}

void xMBPortSerialPutByte( UCHAR ucByte )
{
    UDR = ucByte;
}

void xMBPortSerialGetByte( UCHAR * pucByte )
{
    *pucByte = UDR;
}

void xMBPortSerialGetByteWait( UCHAR * pucByte )
{
	while ( !(UCSR1A & (1 << RXC1)) );

	*pucByte = UDR;
}

ISR (SIG_USART_DATA)
{ 
    xMBRTUTransmitFSM();
}

///////// moved to main.c to allow receive data in non-modbus modes /////
//SIGNAL( SIG_USART_RECV )
//{
//    pxMBFrameCBByteReceived(  );
//}

ISR (SIG_UART_TRANS)
{
	RTS_LOW;
}

