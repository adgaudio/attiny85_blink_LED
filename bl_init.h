#ifndef __BL_INIT_H__
#define __BL_INIT_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>


/* Sets the clock to 8mhz by setting the prescalar to 1.
 * Overrides any fuses that might be set */
#define F_CPU 8000000
#define bl_init_8mhz_clock() {\
  cli(); /* disable interrupts */\
  CLKPR = _BV(CLKPCE); /* lets us configure the clock prescalar within 4 clock cycles */\
  CLKPR = 0x00; /* set the clock prescalar to divide by 1 (on attiny85, this means max clock speed of 8mhz)*/\
  sei(); /* enable interrupts */\
}

/*
 * The power button triggers an interrupt that should wake from sleep
 */
#define bl_init_power_button_interrupt() {\
  GIMSK &= ~_BV(INT0);  /* disable INT0 interrupt */\
  GIMSK |= _BV(PCIE);  /* enable all PCINT interrupts (pin change interrupt) */\
  PCMSK = 0; /* start out with all PCINT interrupts disabled */\
  /* PORTB &= ~_BV(PB2); */ /* set interrupt pin pull HIGH */\
}


void bl_init_disable_peripherals();

void bl_init_led_pwm_and_timer0_interrupt();

#endif /* __BL_INIT_H__ */
