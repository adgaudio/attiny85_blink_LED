#include "bl_init.h"

void bl_init_led_pwm_and_timer0_interrupt() {
  /*
   * Simplified from these great instructions to enable PWM on 4 pins:  http:www.technoblogy.com/show?LE0
   *
   * Timer/Counter0 configuration:  (current value of timer0 is TCNT0)
   * (for some reason, configuration for this timer a bit different than for timer1)
   *
   * COM0A[1:0] == 11:  configures PB0 such that COM0A0 is set to clear OC0A on compare match
   * COM0B[1:0] == 11:  configures PB1 such that COM0B0 is set to clear OC0B on compare match
   * WGM[1:0] == 11 and WGM02 == 0:  together, these settings set Fast PWM mode.
   * CS0[1:0] == 11: set prescalar to 64 (overflow once every 1 / (8mhz/64) * 256 seconds) or 2.048ms
   *
   *
   * Timer0 interrupts (also, see associated ISR functions, ISR(TIMER0_COMPA_vect) and ISR(TIMER0_OVF_vect)
   * TOIE0 = 1: enables timer0 overflow interrupt
   */
  // Configure counter/timer0 for fast PWM on PB0 and PB1
  TCCR0A = 3<<COM0A0 | 3<<COM0B0 | 3<<WGM00;
  TCCR0B = 0<<WGM02 | 3<<CS00;

  // Finally, must set pin modes to output
  DDRB |= _BV(PB0); /* set led pin to output mode */

  DDRB &= ~_BV(PB2);  /* set button pin to input mode */
  PORTB &= ~_BV(PB2);  /* set pull low */

  // As an extra, also use this timer to trigger a timer interrupt
  TIMSK |= _BV(TOIE0);
}


// TODO: power off unused components (PRR register) for power savings
void bl_init_disable_peripherals() {
  wdt_disable();  /* turn off watchdog (for power savings)  // TODO: does this reduce consumption? */
  power_adc_disable();  /* turn off adc  // TODO: does this reduce consumption? */
}
