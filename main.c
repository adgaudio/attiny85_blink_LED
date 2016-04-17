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

#include "blink.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>


void init_led_pwm_and_timer0_interrupt() {
  /*
   * Based on these instructions to enable PWM on 4 pins:  http:www.technoblogy.com/show?LE0
   *
   * Timer/Counter0 configuration:  (current value of timer0 is TCNT0) (for some reason, avr made this timer a bit messy)
   *
   * COM0A[1:0] == 11:  configures PB0 such that COM0A0 is set to clear OC0A on compare match
   * COM0B[1:0] == 11:  configures PB1 such that COM0B0 is set to clear OC0B on compare match
   * WGM[1:0] == 11 and WGM02 == 0:  together, define Fast PWM mode.
   * CS0[1:0] == 11: set prescalar to 64 (overflow once every 1 / (8mhz/64) * 256 seconds) or 2.048ms
   *
   *
   * Timer1 interrupts (need to define associated ISR functions, ISR(TIMER1_COMPA_vect) and ISR(TIMER1_OVF_vect)
   * TOIE0 = 1: enables timer0 overflow interrupt
   */
  // Configure counter/timer0 for fast PWM on PB0 and PB1
  TCCR0A = 3<<COM0A0 | 3<<COM0B0 | 3<<WGM00;
  TCCR0B = 0<<WGM02 | 3<<CS00;

  // Finally, must set pin modes to output
  DDRB |= _BV(PB0); /* set pin to output mode */

  // As an extra, also use this timer to trigger a timer interrupt
  TIMSK |= _BV(TOIE0);
}

ISR(TIMER0_OVF_vect) {
  // TODO: put debounce code here to determine if a click happens
  OCR0A += 1;
// Service routine called by a timer interrupt
// TODO
/* bool DebounceSwitch2() */
/* { */
    /* static uint16_t State = 0; // Current debounce status */
    /* State=(State<<1) | !RawKeyPressed() | 0xe000; */
    /* if(State==0xf000)return TRUE; */
    /* return false; */
// } 
}

/* Sets the clock to 8mhz by setting the prescalar to 1.
 * Overrides any fuses that might be set */
#define F_CPU 8000000
#define init_8mhz_clock() {\
  cli(); /* disable interrupts */\
  CLKPR = _BV(CLKPCE); /* lets us configure the clock prescalar within 4 clock cycles */\
  CLKPR = 0x00; /* set the clock prescalar to divide by 1 (on attiny85, this means max clock speed of 8mhz)*/\
  sei(); /* enable interrupts */\
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


typedef void (*blink_routine)();
blink_routine all_routines[3] = {blink_sos, blink_smooth, blink_ismooth};
volatile int routine = 0;
/* volatile blink_routine routine = blink_sos; */



/* TODO:tasks */
// (done) 1. pwm output on led
// 2. reset button (long press)
//   -- use timer interrupt with debounce algo on the power button pin to determine either power off or next mode
// 3. different pwm blinking modes depending on reset button
// 4. sleep mode (for power saving)
// 5. power off unused components (PRR register)
int main (void) {

  // DDRB = 0;  // start with no output pins
  init_8mhz_clock();
  init_led_pwm_and_timer0_interrupt();  // init fast pwm on pin 0
  init_power_button_interrupt();

  // TODO: power off unused components (PRR register) for power savings
  wdt_disable();  // turn off watchdog (for power savings)  // TODO: does this reduce consumption?
  power_adc_disable();  // turn off adc  // TODO: does this reduce consumption?


  // flash a startup sequence
  blink_led(150, 100);
  blink_led(300, 100);
  blink_led(300, 100);
  blink_led(150, 500);

    /* blink_smooth(); */
    /* blink_ismooth(); */
    /* blink_led(0, 0); */
  while (1) {
    /* routine(); */
    // all_routines[routine]();
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
  // routine = routine % 2 + 1;  //(routine+1);
  blink_smooth();
  // OCR0A = (OCR0A + 50) % 255;
  // we just woke up from sleep and will resume where we left off
}
