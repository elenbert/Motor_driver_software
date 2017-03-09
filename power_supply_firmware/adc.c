#include <avr/io.h>

void init_adc()
{
	ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2); // prescaler = 128
	ADMUX = (1 << REFS0);
}

uint8_t read_adc_data(int first_channel) {
	if (first_channel) {
		ADMUX &= ~(1 << MUX0);
	} else {
		ADMUX |= (1 << MUX0);
	}

	ADCSRA &= ~_BV(ADIF);			// clear hardware "conversion complete" flag
	ADCSRA |= (1 << ADSC);			// start conversion

	while (ADCSRA & _BV(ADSC));		// wait until conversion complete
 
	return ADC;						// read ADC (full 10 bits);
}

