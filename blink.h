#ifndef __BLINK_H__
#define __BLINK_H__

#include <util/delay.h>

#define blink_led(ms, off_ms) {\
  OCR0A = 255;\
  _delay_ms(ms);\
  OCR0A = 0;\
  _delay_ms(off_ms);\
}

void blink_smooth();
void blink_ismooth();
void blink_sos();

#endif /* __BLINK_H__ */
