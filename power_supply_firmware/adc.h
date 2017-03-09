#ifndef ADC_H
#define ADC_H

#define FIRST_ADC_CHANNEL 1
#define SECOND_ADC_CHANNEL 0

void init_adc();
uint8_t read_adc_data(int first_channel);

#endif
