#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "sound_errors.h"
#include "power.h"
#include "led.h"
#include "adc.h"
#include "usart.h"
#include "comm_proto.h"

static int usart_message_started;

void sys_init()
{
	init_leds();
	init_sound();
	init_adc();

	init_usart();
	switch_usart_to_rx();

	init_power_supply();

	/* configure external interrupts */
	GICR = (1 << INT0) | (1 << INT1);
	MCUCR = (1 << ISC01) | (1 << ISC00) | (1 << ISC11) | (1 << ISC10);

	usart_message_started = 0;

	/* activate interrupts */
	sei();
}

static void failure5v()
{
	while (1) {
		leds_g_blink();
		sound_5v_failure();
		_delay_ms(1000);
	}
}

static void failure12v() {
	while (1) {
		leds_r_blink();
		sound_12v_failure();
		_delay_ms(1000);
	}
}

static void failure12v5v()
{
	while (1) {
		leds_rg_blink();
		sound_12v_5v_failure();
		_delay_ms(1000);
	}
}

static void say_to_usart()
{
	switch_usart_to_tx();

	usart_send_str("abc\n");
//	usart_send_char('a');

	_delay_ms(50);

	switch_usart_to_rx();
}

/*
 * Main program entry
 */
int main()
{
	sys_init();

	leds_hello_blink();

	sound_boot_ok();

	while(1) {
		while (!is_work_allowed_by_external_block()) {
			if (is_power_supply_enabled()) {
				disable_high_voltage();
				disable_power_supply();
			}

			sound_external_block();
			leds_rgy_blink();

			_delay_ms(1000);
		}

		if (!is_power_supply_enabled()) {
			enable_power_supply();
	
			_delay_ms(10);
			_delay_ms(1000);

			int ok5v = is_external_5v_ok();
			int ok12v = is_external_12v_ok();

			if (ok5v && ok12v) {
				enable_high_voltage();

				enable_red_led();
				enable_green_led();
				enable_yellow_led();

				sound_ps_enabled();

			} else {
				disable_power_supply();

				if (!ok5v && !ok12v) {
					failure12v5v();
				}

				if (!ok5v) {
					failure5v();
				}

				if (!ok12v) {
					failure12v();
				}
			}
		}
 
		say_to_usart();
		_delay_ms(25);
	}
}

int double_check_5v_ok()
{
	int res = is_external_5v_ok();

	_delay_ms(50);

	res = is_external_5v_ok();

	return res;
}

int double_check_12v_ok()
{
	int res = is_external_12v_ok();

	_delay_ms(50);

	res = is_external_12v_ok();

	return res;
}

/* 5v failure handler */
ISR(INT0_vect)
{
	if (!is_power_supply_enabled()) {
		return;
	}

	if (!double_check_5v_ok()) {
		disable_high_voltage();
		disable_power_supply();
		disable_all_leds();
		failure5v();
	}
}

/* 12 failure handler */
ISR(INT1_vect)
{
	if (!is_power_supply_enabled()) {
		return;
	}

	if (!double_check_12v_ok()) {
		disable_high_voltage();
		disable_power_supply();
		disable_all_leds();
		failure12v();	
	}
}

ISR(USART_RXC_vect)
{
	unsigned char rxd = UDR;

	switch (rxd) {
		case MESSAGE_START:
			usart_message_started = 1;
			break;

		case MESSAGE_END:
			usart_message_started = 0;
			break;
	}
}

