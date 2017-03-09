#include <avr/io.h>
#include <util/delay.h>

#define RED_LED_PIN PB0
#define GREEN_LED_PIN PB1
#define YELLOW_LED_PIN PB2

void init_leds()
{
	DDRB |= (1 << RED_LED_PIN);
	DDRB |= (1 << GREEN_LED_PIN);
	DDRB |= (1 << YELLOW_LED_PIN);
}

/*
 * LED's functionality
 */
void enable_red_led()
{
	PORTB |= (1 << RED_LED_PIN);
}

void disable_red_led()
{
	PORTB &= ~(1 << RED_LED_PIN);
}

void enable_green_led()
{
	PORTB |= (1 << GREEN_LED_PIN);
}

void disable_green_led()
{
	PORTB &= ~(1 << GREEN_LED_PIN);
}

void enable_yellow_led()
{
	PORTB |= (1 << YELLOW_LED_PIN);
}

void disable_yellow_led()
{
	PORTB &= ~(1 << YELLOW_LED_PIN);
}

void disable_all_leds()
{
	disable_red_led();
	disable_green_led();
	disable_yellow_led();
}

void leds_hello_blink()
{
	enable_red_led();
	_delay_ms(300);
	disable_red_led();

	_delay_ms(300);
	enable_green_led();
	_delay_ms(300);
	disable_green_led();

	_delay_ms(300);
	enable_yellow_led();
	_delay_ms(300);
	disable_yellow_led();
}

void leds_rgy_blink()
{
	enable_red_led();
	enable_green_led();
	enable_yellow_led();

	_delay_ms(300);

	disable_red_led();
	disable_green_led();
	disable_yellow_led();
}

void leds_rg_blink()
{
	enable_red_led();
	enable_green_led();

	_delay_ms(300);

	disable_red_led();
	disable_green_led();
}

void leds_r_blink()
{
	enable_red_led();

	_delay_ms(300);

	disable_red_led();
}

void leds_g_blink()
{
	enable_green_led();

	_delay_ms(300);

	disable_green_led();
}

void leds_y_blink()
{
	enable_yellow_led();

	_delay_ms(300);

	disable_yellow_led();
}

