#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#define RED_LED_PIN PD5
#define YELLOW_LED_PIN PB2

#define DEC_MINUS_PWM_PIN PB2
#define DEC_PLUS_PWM_PIN PD5
#define RA_MINUS_PWM_PIN PB3
#define RA_PLUS_PWM_PIN PB4

#define DEC_MINUS_LED_PIN PD4
#define DEC_PLUS_LED_PIN PD6
#define RA_MINUS_LED_PIN PB5 
#define RA_PLUS_LED_PIN PB6

#define RA_PLUS_BUTTON_PIN PD2
#define RA_MINUS_BUTTON_PIN PD3
#define DEC_PLUS_BUTTON_PIN PB0
#define DEC_MINUS_BUTTON_PIN PB1

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1


volatile static int CURRENT_RA_SPEED = 190;
volatile static int CURRENT_DEC_SPEED = 190;

volatile static int RA_PLUS_AXIS_ENABLED = 0;
volatile static int RA_MINUS_AXIS_ENABLED = 0;
volatile static int DEC_PLUS_AXIS_ENABLED = 0;
volatile static int DEC_MINUS_AXIS_ENABLED = 0;

/*
 * IO initialization
 */
void set_out_pins()
{
	DDRD |= (1 << DEC_PLUS_PWM_PIN);
	DDRB |= (1 << DEC_MINUS_PWM_PIN);
	DDRB |= (1 << RA_PLUS_PWM_PIN);
	DDRB |= (1 << RA_MINUS_PWM_PIN);

	DDRB |= (1 << RA_MINUS_LED_PIN);
	DDRB |= (1 << RA_PLUS_LED_PIN);
	DDRD |= (1 << DEC_MINUS_LED_PIN);
	DDRD |= (1 << DEC_PLUS_LED_PIN);
}

void set_in_pins()
{
	/* Configure front panel buttons pins as INPUT */

	DDRD &= ~(1 << RA_PLUS_BUTTON_PIN);
	DDRD &= ~(1 << RA_MINUS_BUTTON_PIN);
	DDRB &= ~(1 << DEC_PLUS_BUTTON_PIN);
	DDRB &= ~(1 << DEC_MINUS_BUTTON_PIN);

	/* Enable internal pull-up resistors for each button pins  */

	PORTD |= (1 << RA_PLUS_BUTTON_PIN);
	PORTD |= (1 << RA_MINUS_BUTTON_PIN);
	PORTB |= (1 << DEC_PLUS_BUTTON_PIN);
	PORTB |= (1 << DEC_MINUS_BUTTON_PIN);
}
/* 
 */


/*
 * Front panel LED functionality
 */
void enable_dec_minus_led()
{
	PORTD |= (1 << DEC_MINUS_LED_PIN);
}

void disable_dec_minus_led()
{
	PORTD &= ~(1 << DEC_MINUS_LED_PIN);
}

void enable_dec_plus_led()
{
	PORTD |= (1 << DEC_PLUS_LED_PIN);
}

void disable_dec_plus_led()
{
	PORTD &= ~(1 << DEC_PLUS_LED_PIN);
}

void enable_ra_minus_led()
{
	PORTB |= (1 << RA_MINUS_LED_PIN);
}

void disable_ra_minus_led()
{
	PORTB &= ~(1 << RA_MINUS_LED_PIN);
}

void enable_ra_plus_led()
{
	PORTB |= (1 << RA_PLUS_LED_PIN);
}

void disable_ra_plus_led()
{
	PORTB &= ~(1 << RA_PLUS_LED_PIN);
}
/*
 *
 */


/*
 * USART initializtion
 */
void USART_Init(unsigned int baud)
{
	UBRRH = (unsigned char)(baud >> 8);
	UBRRL = (unsigned char)baud;

	/* Enable TX, RX and RX interrupts */
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1<<RXCIE);
	UCSRC = (1 << USBS) | (3 << UCSZ0);
}

/*
 * USART helper TX functions
 */
void USART_Transmit(unsigned char data)
{
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = data;
}

void send_Uart_str(unsigned char *s)
{
	while (*s != 0) USART_Transmit(*s++);
}
/*
 *
 */


/*
 * Initialize PWM outputs and timers
 */
void init_pwm()
{
	TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00);
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);

	OCR0A = 0;
	OCR0B = 0;

	OCR1A = 0;
	OCR1B = 0;

	TCCR0B = (1 << CS01) | (1 << CS10);
	TCCR1B = (1 << CS11) | (1 << CS10);
}


/*
 * Startup LED blinking
 */
void hello_led_blink()
{
	enable_ra_minus_led();
	_delay_ms(150);
	disable_ra_minus_led();
	_delay_ms(150);

	enable_ra_plus_led();
	_delay_ms(150);
	disable_ra_plus_led();
	_delay_ms(150);

	enable_dec_plus_led();
	_delay_ms(150);
	disable_dec_plus_led();
	_delay_ms(150);

	enable_dec_minus_led();
	_delay_ms(150);
	disable_dec_minus_led();
	_delay_ms(150);

	enable_ra_minus_led();
	enable_ra_plus_led();
	enable_dec_plus_led();
	enable_dec_minus_led();
	_delay_ms(150);
	disable_ra_minus_led();
	disable_ra_plus_led();
	disable_dec_plus_led();
	disable_dec_minus_led();
}
/*
 *
 */


/*
 * Front panel buttons functionality
 */
int is_ra_plus_button_unpressed()
{
	return PIND & (1 << RA_PLUS_BUTTON_PIN);
}

int is_ra_plus_button_pressed()
{
	return !is_ra_plus_button_unpressed();
}

int is_ra_minus_button_unpressed()
{
	return PIND & (1 << RA_MINUS_BUTTON_PIN);
}

int is_ra_minus_button_pressed()
{
	return !is_ra_minus_button_unpressed();
}

int is_dec_plus_button_unpressed()
{
	return PINB & (1 << DEC_PLUS_BUTTON_PIN);
}

int is_dec_plus_button_pressed()
{
	return !is_dec_plus_button_unpressed();
}

int is_dec_minus_button_unpressed()
{
	return PINB & (1 << DEC_MINUS_BUTTON_PIN);
}

int is_dec_minus_button_pressed()
{
	return !is_dec_minus_button_unpressed();
}
/*
 *
 */


/*
 * Output PWM functionality
 */
void start_ra_plus_axis(const int max_speed)
{
	if (RA_PLUS_AXIS_ENABLED || RA_MINUS_AXIS_ENABLED) {
		return;
	}

	RA_PLUS_AXIS_ENABLED = 1;

	int i;

	for (i = 0; i < max_speed; i++) {
		OCR1A++;
		_delay_ms(4);
	}
}

void stop_ra_plus_axis(const int max_speed)
{
	int i;

	for (i = 0; i < max_speed; i++) {
		OCR1A--;
		_delay_ms(4);
	}

	OCR1A = 0;
	RA_PLUS_AXIS_ENABLED = 0;
}

void start_ra_minus_axis(const int max_speed)
{
	if (RA_PLUS_AXIS_ENABLED || RA_MINUS_AXIS_ENABLED) {
		return;
	}

	RA_MINUS_AXIS_ENABLED = 1;

	int i;

	for (i = 0; i < max_speed; i++) {
		OCR1B++;
		_delay_ms(4);
	}
}

void stop_ra_minus_axis(const int max_speed)
{
	int i;

	for (i = 0; i < max_speed; i++) {
		OCR1B--;
		_delay_ms(4);
	}

	OCR1B = 0;
	RA_MINUS_AXIS_ENABLED = 0;
}

void start_dec_plus_axis(const int max_speed)
{
	if (DEC_PLUS_AXIS_ENABLED || DEC_MINUS_AXIS_ENABLED) {
		return;
	}

	DEC_PLUS_AXIS_ENABLED = 1;

	int i;

	for (i = 0; i < max_speed; i++) {
		OCR0A++;
		_delay_ms(4);
	}
}

void stop_dec_plus_axis(const int max_speed)
{
	int i;

	for (i = 0; i < max_speed; i++) {
		OCR0A--;
		_delay_ms(4);
	}

	OCR0A = 0;
	DEC_PLUS_AXIS_ENABLED = 0;
}

void start_dec_minus_axis(const int max_speed)
{
	if (DEC_PLUS_AXIS_ENABLED || DEC_MINUS_AXIS_ENABLED) {
		return;
	}

	DEC_MINUS_AXIS_ENABLED = 1;

	int i;

	for (i = 0; i < max_speed; i++) {
		OCR0B++;
		_delay_ms(4);
	}
}

void stop_dec_minus_axis(const int max_speed)
{
	int i;

	for (i = 0; i < max_speed; i++) {
		OCR0B--;
		_delay_ms(4);
	}

	OCR0B = 0;
	DEC_MINUS_AXIS_ENABLED = 0;
}
/*
 *
 */

//ISR(WDT_vect) //подпрограмма обработки прерывания от сторожевого таймера
//{
//	wdt_reset(); //сброс сторожевого таймера
//	WDTCSR = 1 << WDIE; //разрешение прерываний от сторожевого таймера (запрещаются автоматически при сбросе сторожевого таймера)
//}

/*
 * Main program entry
 */
int main()
{
//	wdt_enable(WDTO_8S);
//	WDTCSR = (1<<WDIE);

	set_out_pins();

	hello_led_blink();

	set_in_pins();

	USART_Init(MYUBRR);

	init_pwm();

    sei();	

	volatile int ra_plus_bt_enabled = 0;
	volatile int ra_minus_bt_enabled = 0;
	volatile int dec_plus_bt_enabled = 0;
	volatile int dec_minus_bt_enabled = 0;

	/* Just simple forever cycle with pressed buttons processing */
	while(1) {

		if (is_ra_plus_button_pressed()) {
			_delay_ms(500);

			if (is_ra_plus_button_pressed()) {
				if (!ra_minus_bt_enabled) {
					enable_ra_plus_led();
					start_ra_plus_axis(CURRENT_RA_SPEED);
					ra_plus_bt_enabled = 1;
				}
			}
		} else if (is_ra_plus_button_unpressed()) {
			if (ra_plus_bt_enabled) {
				disable_ra_plus_led();
				stop_ra_plus_axis(CURRENT_RA_SPEED);
				ra_plus_bt_enabled = 0;
			}
		}

		if (is_ra_minus_button_pressed()) {
			_delay_ms(500);

			if (is_ra_minus_button_pressed()) {
				if (!ra_plus_bt_enabled) {
					enable_ra_minus_led();
					start_ra_minus_axis(CURRENT_RA_SPEED);
					ra_minus_bt_enabled = 1;
				}
			}
		} else if (is_ra_minus_button_unpressed()) {
			if (ra_minus_bt_enabled) {
				disable_ra_minus_led();
				stop_ra_minus_axis(CURRENT_RA_SPEED);
				ra_minus_bt_enabled = 0;
			}
		}

		if (is_dec_plus_button_pressed()) {
			_delay_ms(500);

			if (is_dec_plus_button_pressed()) {
				if (!dec_minus_bt_enabled) {
					enable_dec_plus_led();
					start_dec_plus_axis(CURRENT_DEC_SPEED);
					dec_plus_bt_enabled = 1;
				}
			}
		} else if (is_dec_plus_button_unpressed()) {
			if (dec_plus_bt_enabled) {
				disable_dec_plus_led();
				stop_dec_plus_axis(CURRENT_DEC_SPEED);
				dec_plus_bt_enabled = 0;
			}
		}

		if (is_dec_minus_button_pressed()) {
			_delay_ms(500);

			if (is_dec_minus_button_pressed()) {
				if (!dec_plus_bt_enabled) {
					enable_dec_minus_led();
					start_dec_minus_axis(CURRENT_DEC_SPEED);
					dec_minus_bt_enabled = 1;
				}
			}
		} else if (is_dec_minus_button_unpressed()) {
			if (dec_minus_bt_enabled) {
				disable_dec_minus_led();
				stop_dec_minus_axis(CURRENT_DEC_SPEED);
				dec_minus_bt_enabled = 0;
			}
		}

		_delay_ms(1);
	}
}


/*
 * USART interrupt handlder with data processing
 */
ISR(USART_RX_vect)
{
	unsigned char rxd = UDR;

	if (rxd == 0xF2) {
		enable_ra_plus_led();
		start_ra_plus_axis(CURRENT_RA_SPEED);
		send_Uart_str("OK\n");

	} else if (rxd == 0xF3) {
		disable_ra_plus_led();
		stop_ra_plus_axis(CURRENT_RA_SPEED);
		send_Uart_str("OK\n");

	} else if (rxd == 0xF4) {
		enable_ra_minus_led();
		start_ra_minus_axis(CURRENT_RA_SPEED);
		send_Uart_str("OK\n");

	} else if (rxd == 0xF5) {
		disable_ra_minus_led();
		stop_ra_minus_axis(CURRENT_RA_SPEED);
		send_Uart_str("OK\n");

	} else if (rxd == 0xF8) {
		enable_dec_plus_led();
		start_dec_plus_axis(CURRENT_DEC_SPEED);
		send_Uart_str("OK\n");

	} else if (rxd == 0xF9) {
		disable_dec_plus_led();
		stop_dec_plus_axis(CURRENT_DEC_SPEED);
		send_Uart_str("OK\n");

	} else if (rxd == 0xFA) {
		enable_dec_minus_led();
		start_dec_minus_axis(CURRENT_DEC_SPEED);
		send_Uart_str("OK\n");

	} else if (rxd == 0xFB) {
		disable_dec_minus_led();
		stop_dec_minus_axis(CURRENT_DEC_SPEED);
		send_Uart_str("OK\n");
	}
}

