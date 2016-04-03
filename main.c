/*
 * Program an ATTiny85 chip to control a LED.
 *
 * Features:
 *   - on-off momentary button
 *   - dimmable led, with different blinking modes
 *
 * Help from various sources:
 * - fast PWM:  http://www.re-innovation.co.uk/web12/index.php/en/blog-75/305-fast-pwm-on-attiny85
 *
 * The connections to the ATTiny are as follows:
 * ATTinyInfo
 * Pin  1   - RESET / Rx (Not receiving any data)
 * Pin  2   - B3
 * Pin  3   - B4
 * Pin  4   - GND
 * Pin  5   - B0 -- 0CR0A  led PWM pin
 * Pin  6   - B1 -- OCR0B  led PWM pin  (only if DDRB enables it!)
 * Pin  7   - B2
 * Pin  8   - +Vcc
 */

#include <avr/io.h>
#include <util/delay.h>

/*
 * fast pwm configuration (see link in comments above)
 *
 *   COMOA0/COMOB0 to 2 (binary 10): Compare Match Output Mode needs to be 'Clear on Match. Set at Bottom'
 *   WGM0[2:0] to 3 (binary 011):  this is the Waveform Generation Mode which we set to Fast PWM.
 *   CS00/CS10 = 1: This tells Timer 0/1 NOT to use a prescaler.
 *   PWM1B = 0: This enables the use of Output Compare Unit B from Timer 1 (OC1B).
 *   PWM1A = 1: This disables the use of Output Compare Unit A from Timer 1 (OC1A) as that pin is shared with OC0B.
 */
#define init_pwm(pin) {\
  TCCR0A = 2<<COM0A0 | 2<<COM0B0 | 3<<WGM00;\
  TCCR0B = 0<<WGM02 | 1<<CS00;\
  TCCR1 = 0<<PWM1A | 0<<COM1A0 | 1<<CS10;\
  GTCCR = 1<<PWM1B | 2<<COM1B0;\
  DDRB |= 1<<pin; /* set pin to output mode */\
}

/* tasks */
// (done) 1. pwm output on led
// 2. reset button
//   --> bit_is_set(...)
// 3. different pwm blinking modes depending on reset button
// 4. sleep mode (for power saving)
int main (void) {
  DDRB = 0;  // start with no output pins

  init_pwm(DDB0);  // init fast pwm on pin 0
  uint8_t ledPWM = 0;
  int8_t dir = 1;

  while (1) {
    _delay_ms(0.45);

    /*
     * brightness leds
     */
    ++ledPWM; // overflow wraps around from 255 to 0
    OCR0A = ledPWM;
    // OCR0B = 10;
    // OCR1B = 10;
    // _delay_ms(100);
    // OCR0A = 200;
    // OCR0B = 200;
    // OCR1B = 200;

    // _delay_ms(100);
    // // OCR0A  = 0x10;
    // OCR0A  = 0x10;
    // // OCR1B  = 0x10;
    // _delay_ms(100);
    // // OCR0A  = 0x30;
    // OCR0A  = 0x20;
    // // OCR1B  = 0x20;
    // _delay_ms(100);
    // // OCR0A  = 0xA0;
    // OCR0A  = 0xA0;
    // // OCR1B  = 0xA0;
    // _delay_ms(100);
    // // OCR0A  = 0xFF;
    // OCR0A  = 0xFF;
    // OCR1B  = 0xFF;

    /*
    blink leds (not on pwm pin)
    */
    // _delay_ms(10);
    // PORTB = _BV(PB4);
    // PORTB = ~_BV(PB3);
    // _delay_ms(10);
    // PORTB = ~_BV(PB4);
    // PORTB = _BV(PB3);
  }

  return 1;
}


/*
 * Useful examples of how to set/clear pins
 */
// void set_pins (void) {
//
//
// FIRST, set pins to be output
//
// DDRB = 0b00001000;
// DDRB = _BV(PB4) | _BV(PB3) | _BV(PB1) | _BV(PB2);
// DDRB = 1<<DDB0 | 1<<DDB1 | 1<<DDB2 | 1<<DDB3 | 1<DDB4 | 1<<DDB5;
// DDRB = 1<<DDB0 | 1<<DDB1 | 1<<DDB2 | 1<<DDB3 | 1<DDB4 | 1<<DDB5;
// DDRB = 1<<DDB0 | 1<<DDB1;  // enable both PWM pins
//
//THEN, you can set high/low:
// // flash# 1:
// // set PB3 high
// [> PORTB = 0b00001000; <]
// [> PORTB |= _BV(PB3) <]
// [> PORTB = _BV(PB3); <]

// if (bit_is_clear(PINB,3)) {
  // PORTB = _BV(PB3);
// }

// _delay_ms(10);
// // set PB3 low
// [> PORTB = 0b00000000; <]
// [> PORTB &= ~_BV(PB3) <]
// PORTB = ~_BV(PB3);
// _delay_ms(19);
// }
