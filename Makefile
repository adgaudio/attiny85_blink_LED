DEVICE=attiny85

AVRDUDE = avrdude -c avrisp -p $(DEVICE) -P /dev/ttyUSB0 -b 19200
COMPILE = avr-gcc -Wall -Os -DF_CPU=8000000 -mmcu=$(DEVICE)

HEADER_FILES = $(wildcard %.h)

default: build flash


# any time a header file changes, recompile everything... eww
%.o: %.c $(HEADER_FILES)
	$(COMPILE) -c $< -o $@


build: main.o
	$(COMPILE) -o main.elf main.o

	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf

flash:
	$(AVRDUDE) -U flash:w:main.hex:i

clean:
	rm -f main.hex main.elf main.o

