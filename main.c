/*
 * Program an ATTiny85 chip to control a LED.
 *
 * Features:
 *   - on-off momentary button
 *   - dimmable led, with different blinking modes
 *
 * Help from various sources:
 * - attiny85 datasheet:  http://www.atmel.com/images/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf
 * - avr libc reference manual: http://www.atmel.com/webdoc/AVRLibcReferenceManual/
 * - fast PWM (led):  http://www.re-innovation.co.uk/web12/index.php/en/blog-75/305-fast-pwm-on-attiny85
 *
 * The connections to the ATTiny are as follows:
 * ATTinyInfo
 * Pin  1   - RESET / Rx (Not receiving any data)
 * Pin  2   - B3
 * Pin  3   - B4
 * Pin  4   - GND
 * Pin  5   - B0 -- 0CR0A  led PWM pin
 * Pin  6   - B1 -- OCR0B  led PWM pin  (only if DDRB enables it!)
 * Pin  7   - B2 -- INT0
 * Pin  8   - +Vcc
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

/*
 * fast pwm configuration for blinking an led (see link in comments above)
 *
 *   COMOA0/COMOB0 to 2 (binary 10): Compare Match Output Mode needs to be 'Clear on Match. Set at Bottom'
 *   WGM0[2:0] to 3 (binary 011):  this is the Waveform Generation Mode which we set to Fast PWM.
 *   CS00/CS10 = 1: This tells Timer 0/1 NOT to use a prescaler.
 *   PWM1B = 0: This enables the use of Output Compare Unit B from Timer 1 (OC1B).
 *   PWM1A = 1: This disables the use of Output Compare Unit A from Timer 1 (OC1A) as that pin is shared with OC0B.
 */
#define init_led_pwm(pin) {\
  TCCR0A = 2<<COM0A0 | 2<<COM0B0 | 3<<WGM00;\
  TCCR0B = 0<<WGM02 | 1<<CS00;\
  TCCR1 = 0<<PWM1A | 0<<COM1A0 | 1<<CS10;\
  GTCCR = 1<<PWM1B | 2<<COM1B0;\
  DDRB |= _BV(pin); /* set pin to output mode */\
}

#define blink_led(ms, off_ms) {\
  OCR0A = 255;\
  _delay_ms(ms);\
  OCR0A = 0;\
  _delay_ms(off_ms);\
}

/*
 * The power button triggers an interrupt that should wake from sleep
 */
#define init_power_button_interrupt() {\
  GIMSK &= ~_BV(INT0);  /* disable INT0 interrupt */\
  GIMSK |= _BV(PCIE);  /* enable all PCINT interrupts (pin change interrupt) */\
  PCMSK = _BV(PCINT2);  /* enable only PCINT2 from available pin change interrupts */\
  PORTB &= ~_BV(PB2); /* set interrupt pin pull HIGH */\
  sei();  /* enable any interrupts */\
}

/* put the chip into sleep mode, but listen for interrupt.
 */
void power_down() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();  // disable interrupts
    sleep_enable();
    sleep_bod_disable();
    sei();  // enable interrupts

    sleep_cpu();
    sleep_disable();
}

/* smooth on and off */
void blink_smooth() {
  uint8_t ledPWM;
  for (ledPWM=0; ledPWM<255; ledPWM++) {
    OCR0A = ledPWM;
    _delay_ms(0.45);
  }
  for (ledPWM=255; ledPWM>0; ledPWM--) {
    OCR0A = ledPWM;
    _delay_ms(0.45);
  }
}

void blink_ismooth() {
  uint8_t ledPWM;
  for (ledPWM=255; ledPWM>0; ledPWM--) { OCR0A = ledPWM; _delay_ms(0.45); }
  for (ledPWM=0; ledPWM<255; ledPWM++) { OCR0A = ledPWM; _delay_ms(0.45); }
}

void blink_sos() {
  int i;
  for (i=0; i<3; i++) { blink_led(10, 10); }
  for (i=0; i<3; i++) { blink_led(40, 20); }
  for (i=0; i<3; i++) { blink_led(10, 10); }
  _delay_ms(100);
}

/* TODO:tasks */
// (done) 1. pwm output on led
// 2. reset button (long press)
// 3. different pwm blinking modes depending on reset button
// 4. sleep mode (for power saving)
// 5. power off unused components (PRR register)
int main (void) {

  // DDRB = 0;  // start with no output pins

  init_led_pwm(DDB0);  // init fast pwm on pin 0
  init_power_button_interrupt();

  // TODO: power off unused components (PRR register) for power savings
  wdt_disable();  // turn off watchdog (for power savings)  // TODO: does this reduce consumption?
  power_adc_disable();  // turn off adc  // TODO: does this reduce consumption?


  // flash a startup sequence
  blink_led(15, 10);
  blink_led(30, 10);
  blink_led(30, 10);
  blink_led(15, 50);

  while (1) {
    blink_smooth();
    power_down();
    blink_ismooth();
    blink_led(0, 0);

    power_down();
  }

  return 1;
}

ISR(BADISR_vect) {
  /* Interrupt Routine that gets triggered if an interrupt happens but the
   * routine for it isn't defined.
   *
   * This exists just to warn for a code error that there is an unhandled
   * interrupt */
  blink_sos();
}

ISR(PCINT0_vect) {  // this captures all pcint changes on attiny85
  // we just woke up from sleep and will resume where we left off
}

// ISR(PCINT2_vect) {
  // blink_led(2, 2);
// }
// ISR(INT0_vect)
// {
  // blink_led(2, 2);
  // // blink_led(90, 20);
// }


