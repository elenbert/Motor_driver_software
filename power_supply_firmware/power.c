#include <avr/io.h>
#include <util/delay.h>

#define RELAY_220_PIN PB3
#define RELAY_110_PIN PB4
#define RELAY_110_DISCHARGE_PIN PB5

#define EXTERNAL_BLOCK_PIN PD4
#define EXTERNAL_MON_5V_PIN PD2
#define EXTERNAL_MON_12V_PIN PD3

volatile static int POWER_SUPPLY_ENABLED = 0;

/*
 * IO initialization
 */
void init_power_supply()
{
	/* set relay control pins as output */
	DDRB |= (1 << RELAY_220_PIN);
	DDRB |= (1 << RELAY_110_PIN);
	DDRB |= (1 << RELAY_110_DISCHARGE_PIN);

	/* Configure optocouplers pins as INPUT */
	DDRD &= ~(1 << EXTERNAL_BLOCK_PIN);
	DDRD &= ~(1 << EXTERNAL_MON_5V_PIN);
	DDRD &= ~(1 << EXTERNAL_MON_12V_PIN);

	/* Enable internal pull-up resistors for each optocoupler pins  */
	PORTD |= (1 << EXTERNAL_BLOCK_PIN);
	PORTD |= (1 << EXTERNAL_MON_5V_PIN);
	PORTD |= (1 << EXTERNAL_MON_12V_PIN);
}

/*
 * Relays functionality
 */
inline void enable_220_relay()
{
	PORTB |= (1 << RELAY_220_PIN);
}

inline void disable_220_relay()
{
	PORTB &= ~(1 << RELAY_220_PIN);
}

inline void enable_110_relay()
{
	PORTB |= (1 << RELAY_110_PIN);
}

inline void disable_110_relay()
{
	PORTB &= ~(1 << RELAY_110_PIN);
}

inline void disable_110_discharge_unit()
{
	PORTB |= (1 << RELAY_110_DISCHARGE_PIN);
}

inline void enable_110_discharge_unit()
{
	PORTB &= ~(1 << RELAY_110_DISCHARGE_PIN);
}

/*
 * External monitors
 */
int is_work_allowed_by_external_block()
{
	return PIND & (1 << EXTERNAL_BLOCK_PIN);
}

int is_external_5v_ok()
{
	return !(PIND & (1 << EXTERNAL_MON_5V_PIN));
}

int is_external_12v_ok()
{
	return !(PIND & (1 << EXTERNAL_MON_12V_PIN));
}

/*
 * Power supply functionality
 */
void enable_power_supply()
{
	_delay_ms(1000);
	enable_220_relay();

	POWER_SUPPLY_ENABLED = 1;
}

void enable_high_voltage()
{
	disable_110_discharge_unit();
	_delay_ms(600);
	enable_110_relay();	
}

void disable_power_supply()
{
	_delay_ms(10);
	disable_220_relay();
	POWER_SUPPLY_ENABLED = 0;
}

void disable_high_voltage()
{
	disable_110_relay();
	_delay_ms(10);
	enable_110_discharge_unit();
}

int is_power_supply_enabled()
{
	return POWER_SUPPLY_ENABLED;
}

