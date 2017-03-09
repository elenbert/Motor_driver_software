
PROGRAM = pwm
SRC := main.c
CFLAGS := -mmcu=attiny2313 -Os -DF_CPU=8000000

CC := avr-gcc
OBJCOPY := avr-objcopy

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(SRC) -o $(PROGRAM)
	$(OBJCOPY) -j .text -j .data -O ihex $(PROGRAM) $(PROGRAM).hex

upload:
	avrdude -F -p t2313 -P /dev/ttyUSB0 -c stk500 -U flash:w:$(PROGRAM).hex

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o $(PROGRAM).hex
