#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "usart.h"

#define TRANCIEVER_CONTROL_PIN PD5

#define BAUD 9600

void init_usart()
{
	DDRD |= (1 << TRANCIEVER_CONTROL_PIN);

	unsigned int ubbr = (unsigned int) (F_CPU/16/BAUD - 1);

	UBRRH = (unsigned char)(ubbr >> 8);
	UBRRL = (unsigned char)ubbr;

	/* Enable TX, RX and RX interrupts */
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
}

/*
 * USART helper TX functions
 */
void usart_send_char(char c)
{
	while (!(UCSRA & (1 << UDRE)));

	UDR = c;
}

void usart_send_str(char *s)
{
	while (*s) {
		usart_send_char(*s);
		s++;
	}
}

void switch_usart_to_tx()
{	
	PORTD |= (1 << TRANCIEVER_CONTROL_PIN);
}

void switch_usart_to_rx()
{
	PORTD &= ~(1 << TRANCIEVER_CONTROL_PIN);
}

/*
 * USART interrupt handlder with data processing
 */
//	unsigned char rxd = UDR;

//	if (rxd == 0x1C) {
//		switch_usart_to_tx();

//		uint32_t adc_res = adc_read(0);
//		unsigned long real_ra_voltage = ((unsigned long)adc_res * v_ref) / (unsigned long)adc_conv_s;

//		uint32_t real_ra_voltage_intpart = (uint32_t)real_ra_voltage;
//		uint32_t real_ra_voltage_decpart = (real_ra_voltage - real_ra_voltage_intpart) * 100;

//		real_ra_voltage_decpart = 123;

//		char tx_buf[30] = {0};
//		memcpy(tx_buf, &real_ra_voltage_intpart, sizeof(real_ra_voltage_intpart));
//		memcpy(tx_buf, &real_ra_voltage_decpart, sizeof(real_ra_voltage_decpart));

//		uint32_t ra_adc_res = adc_read(0);
//		uint32_t dec_adc_res = adc_read(1);

//		snprintf(tx_buf, sizeof(tx_buf), "ra = %u\n", ra_adc_res);
//		send_usart_str(tx_buf);

//		memset(tx_buf, 0, sizeof(tx_buf));

//		snprintf(tx_buf, sizeof(tx_buf), "dec = %u\n", dec_adc_res);
//		send_usart_str(tx_buf);

//		_delay_ms(160); // time to complete transmission

//		switch_usart_to_rx();

//		_delay_ms(500);
//	}
//}

