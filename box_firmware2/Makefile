
PROGRAM = pwm

SRC := main.c motors.c fp_leds.c fp_buttons.c utils.c modbus/mb.c events.c ps_controller.c \
modbus/functions/mbfuncholding.c modbus/functions/mbfuncinput.c \
modbus/functions/mbutils.c modbus/rtu/mbcrc.c modbus/rtu/mbrtu.c \
modbus/port/portevent.c modbus/port/portserial.c modbus/port/porttimer.c 

CFLAGS := -Imodbus -Imodbus/include/ -Imodbus/rtu -mmcu=atmega162 -Os -DF_CPU=8000000 -DRTS_ENABLE -Wall -gstabs -Wall #-Wstrict-prototypes
CFLAGS += -Wp,-M,-MP,-MT,$(*F).o,-MF,dep/$(@F).d

## Linker flags
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref,-gc-sections

CC := avr-gcc
OBJCOPY := avr-objcopy

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(PROGRAM)
	$(OBJCOPY) -j .text -j .data -R .eeprom -O ihex $(PROGRAM) $(PROGRAM).hex
#	$(OBJCOPY) -O ihex -j .eeprom --change-section-lma .eeprom=0 $(PROGRAM) $(PROGRAM).eep

readfuse:
	avrdude -p m162 -c usbasp -U lfuse:r:-:i -U hfuse:r:-:i -U efuse:r:-:i -U lock:r:-:i

writefuse:
	avrdude -p m162 -c usbasp -U lfuse:w:0xfd:m -U hfuse:w:0x99:m -U efuse:w:0xf9:m

upload:
	avrdude -p m162 -c usbasp -U flash:w:$(PROGRAM).hex
	avrdude -p m162 -c usbasp -U eeprom:w:$(PROGRAM).eep
	avr-size -C --mcu=atmega162 $(PROGRAM)

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o $(PROGRAM).hex

-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)
