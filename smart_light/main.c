/*
 * Program an ATTiny85 chip to control a LED based on accelerometer readings
 * Designed for a rear bike light.
 *
 * Features:
 *   - detect "braking" and change mode while braking
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

#include "bl_init.h"
#include "bl_power.h"
#include "bl_debounce.h"
#include "bl_blink.h"

#include <avr/wdt.h>

volatile int bl_routine = 0;



/* TODO:tasks */
// 5. power off unused components (PRR register)
int main (void) {

  // DDRB = 0;  // start with no output pins
  bl_init_8mhz_clock();
  bl_init_led_pwm_and_timer0_interrupt();  // init fast pwm on pin 0
  bl_init_power_button_interrupt();
  bl_init_disable_peripherals();

  // flash a startup sequence
  bl_blink_led(75, 50);
  bl_blink_led(150, 50);
  bl_blink_led(150, 50);
  bl_blink_led(75, 100);

  // start powered off
  bl_power_down();
  while (1) {
    bl_blink_all_routines[bl_routine]();
  }

  return 1;
}

ISR(BADISR_vect) {
  /* Interrupt Routine that gets triggered if an interrupt happens but the
   * routine for it isn't defined.
   *
   * This exists just to warn for a code error that there is an unhandled
   * interrupt */
  bl_blink_sos();
}

