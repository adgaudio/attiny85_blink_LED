#include "blink.h"

#include <util/delay.h>

/* smooth on and off */
void blink_smooth() {
  uint8_t ledPWM;
  for (ledPWM=0; ledPWM<255; ledPWM++) {
    OCR0A = ledPWM;
    _delay_ms(5);
  }
  for (ledPWM=255; ledPWM>0; ledPWM--) {
    OCR0A = ledPWM;
    _delay_ms(5);
  }
}

void blink_ismooth() {
  uint8_t ledPWM;
  for (ledPWM=255; ledPWM>0; ledPWM--) { OCR0A = ledPWM; _delay_ms(1); }
  /* for (ledPWM=0; ledPWM<255; ledPWM++) { OCR0A = ledPWM; _delay_ms(0.45); } */
}

void blink_sos() {
  int i;
  for (i=0; i<3; i++) { blink_led(100, 100); }
  for (i=0; i<3; i++) { blink_led(400, 200); }
  for (i=0; i<3; i++) { blink_led(100, 100); }
  _delay_ms(300);
}
