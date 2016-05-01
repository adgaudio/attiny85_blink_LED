#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


/* Put the chip into sleep mode, but listen for interrupt to wake from sleep
 */
void bl_power_down() {

  cli();  // disable interrupts
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  DDRB &= ~_BV(PB0); // disable led pin
  PCMSK |= _BV(PCINT2);  /* enable PCINT2 pin change interrupt */

  sei();  // enable interrupts
  sleep_mode();

  DDRB |= _BV(PB0);  // enable led pin
  PCMSK &= ~_BV(PCINT2);  /* disable PCINT2 pin change interrupt */
}


ISR(PCINT0_vect) {
  /* Interrupt Routine that is activated to wake from sleep
   * captures all pcint changes on attiny85. */
  uint8_t prev_OCR0A = OCR0A;
  bl_blink_ismooth();
  OCR0A = prev_OCR0A;
  // we just woke up from sleep and will resume where we left off
}
