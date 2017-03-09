/*
 * Power supply controller
 * Kutkov Oleg
 * December 2016
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/delay.h>

#define UART_BAUD_RATE 9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) \
    ( ( F_OSC ) / ( ( UART_BAUD_RATE ) * 16UL ) - 1 )


/*
 * PS commands
 */
#define PS_WRITE_CMD 0x42
#define PS_READ_CMD 0x43

#define PS_ACTIVATE 0x1A
#define PS_DEACTIVATE 0x1B

#define PS_ENABLE 0x2C
#define PS_DISABLE 0x2E

#define PS_COMMAND_OK 0xAC
#define PS_COMMAND_BAD 0xAB

#define PS_PING 0x15
#define PS_PONG 0x16

#define PS_5V_FAILURE 0xBE
#define PS_12V_FAILURE 0xBC
#define PS_5V_12V_FAILURE 0xBB

#define PS_GET_RA_CURRENT 0x10
#define PS_GET_DEC_CURRENT 0x11

#define PS_RA_OVERCURRENT 0x21
#define PS_DEC_OVERCURRENT 0x22

#define PS_RA_NO_CURRENT 0x31
#define PS_DEC_NO_CURRENT 0x32

/*
 */

#define CHECKSUM_MAGIC_CORRECTION 5
#define CHECKSUM_SIMPLE(A, B) (A + B - CHECKSUM_MAGIC_CORRECTION)

#define DATA_BUFFER_LEN 3

static uint8_t tx_data_buffer[DATA_BUFFER_LEN];
static uint8_t tx_data_buf_pos = 0;

static uint8_t rx_data_buffer[DATA_BUFFER_LEN];
static uint8_t rx_data_buf_pos = 0;

static uint8_t frame_received = 0;

static uint16_t ping_delay_counter = 0;
static uint16_t pong_delay_counter = 0;

static uint8_t ps_activated = 0;
static uint8_t ps_enabled = 0;

static uint8_t ps_pong_rcv = 0;

void init_ps_controller()
{
	UBRR0L = UART_BAUD_CALC(UART_BAUD_RATE, F_CPU);

	UCSR0C = (1 << URSEL0) | (1 << UCSZ00) | ( 1 << UCSZ01);

	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << TXCIE0);

	DDRD |= (1 << PD2);

	PORTD &= ~(1 << PD2);
}

static void switch_to_tx()
{
	PORTD |= (1 << PD2);
}

static void switch_to_rx()
{
	PORTD &= ~(1 << PD2);
}

void send_data()
{
	switch_to_tx();

	tx_data_buf_pos = 0;
	UDR0 = tx_data_buffer[0];

	UCSR0B |= (1 << UDRIE0);
}

void send_cmd(int cmd)
{
//	if (tx_data_buf_pos != 0) {
//		return;
//	}

	tx_data_buffer[0] = PS_WRITE_CMD;
	tx_data_buffer[1] = cmd;
	tx_data_buffer[2] = CHECKSUM_SIMPLE(PS_WRITE_CMD, cmd);

	send_data();
}

void init_ps()
{
	send_cmd(PS_ACTIVATE);
}

void enable_ps()
{
	send_cmd(PS_ENABLE);
}

void ps_ping()
{
	ping_delay_counter++;

	if (ping_delay_counter < 33333) {
		return;
	}

	ps_pong_rcv = 0;
	send_cmd(PS_PING);

	ping_delay_counter = 0;
}

void ps_poll()
{
	if (!frame_received) {
		/* no data to process */
		return;
	}

	if (CHECKSUM_SIMPLE(rx_data_buffer[0], rx_data_buffer[1]) != rx_data_buffer[2]) {
		/* invalid checksum */
		return;
	}

	switch (rx_data_buffer[0]) {
		case PS_ACTIVATE:
			ps_activated = (rx_data_buffer[1] == PS_COMMAND_OK);
			break;

		case PS_ENABLE:
			ps_enabled = (rx_data_buffer[1] == PS_COMMAND_OK);
			ps_pong_rcv = 1;
			break;

		case PS_COMMAND_OK:
			ps_pong_rcv = rx_data_buffer[1] == PS_PONG;
			break;

		default:
			break;
	}

	frame_received = 0;
}

int is_ps_online()
{
	pong_delay_counter++;

	if (pong_delay_counter < 65500) {
		return 1;
	}

	pong_delay_counter = 0;

	return ps_pong_rcv;
}

int is_ps_activated()
{
	return ps_activated;
}

int is_ps_enabled()
{
	return ps_enabled;
}

ISR (USART0_RXC_vect)
{
	rx_data_buffer[rx_data_buf_pos++] = UDR0;

	if (rx_data_buf_pos == DATA_BUFFER_LEN) {
		rx_data_buf_pos = 0;
		frame_received = 1;
	}
}

ISR (USART0_TXC_vect)
{
	switch_to_rx();
}

ISR (USART0_UDRE_vect)
{
	tx_data_buf_pos++;

	if (tx_data_buf_pos == DATA_BUFFER_LEN) {
		UCSR0B &= ~(1 << UDRIE0);
		tx_data_buf_pos = 0;
	} else {
		switch_to_tx();
		UDR0 = tx_data_buffer[tx_data_buf_pos];
	}
}

