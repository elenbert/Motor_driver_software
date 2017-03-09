#include <inttypes.h>
#include "adc.h"

static const unsigned long v_ref = 4.82;
static const uint16_t adc_conv_s = 1024;
static const uint16_t amper_volt_zero = 512;  // 1024 adc_conv_s/2
static const uint8_t v_per_amp = 185;

static inline uint8_t get_ra_raw_data()
{
	return read_adc_data(FIRST_ADC_CHANNEL);
}

static inline uint8_t get_dec_raw_data()
{
	return read_adc_data(SECOND_ADC_CHANNEL);
}

static inline unsigned long calculate_adc_voltage(const uint8_t adc_raw_data)
{
	return (adc_raw_data * v_ref) / adc_conv_s;
}

static int calculate_amper()
{
	
}

