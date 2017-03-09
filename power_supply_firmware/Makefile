
PROGRAM = ps
SRC := main.c sound.c sound_errors.c usart.c adc.c ampermeter.c led.c power.c
CFLAGS := -mmcu=atmega8 -Os -DF_CPU=8000000

CC := avr-gcc
OBJCOPY := avr-objcopy

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(SRC) -o $(PROGRAM)
	$(OBJCOPY) -j .text -j .data -O ihex $(PROGRAM) $(PROGRAM).hex

upload:
	avrdude -F -p m8 -P /dev/ttyUSB0 -c stk500 -U flash:w:$(PROGRAM).hex

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o $(PROGRAM).hex
