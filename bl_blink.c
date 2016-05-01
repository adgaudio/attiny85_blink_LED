#include "bl_blink.h"

#include <util/delay.h>
#include <avr/io.h>

/* smooth on and off */
void bl_blink_smooth() {
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

void bl_blink_ismooth() {
  uint8_t ledPWM;
  for (ledPWM=255; ledPWM>150; ledPWM--) { OCR0A = ledPWM; _delay_ms(2); }
  for (ledPWM=150; ledPWM<255; ledPWM++) { OCR0A = ledPWM; _delay_ms(1); }
}

void bl_blink_sos() {
  int i;
  for (i=0; i<3; i++) { bl_blink_led(100, 100); }
  for (i=0; i<3; i++) { bl_blink_led(400, 200); }
  for (i=0; i<3; i++) { bl_blink_led(100, 100); }
  _delay_ms(500);
}

void bl_blink_pattern() {
  bl_blink_led(166, 20);
  bl_blink_led(469, 31);
  bl_blink_ismooth();
  bl_blink_led(202, 167);
}

void bl_blink_on_off_fast() { bl_blink_led(100, 100) };
void bl_blink_on_off_slow() { bl_blink_led(300, 100) };
void bl_blink_anomaly() { bl_blink_led(300, 800) };

void bl_blink_constant_max() { OCR0A = 255; }
void bl_blink_constant_med() { OCR0A = 170; }
void bl_blink_constant_low() { OCR0A = 100; }

const int bl_blink_num_routines = 8;
const bl_blink_routine bl_blink_all_routines[] = {
  // bl_blink_sos,
  bl_blink_ismooth,
  bl_blink_anomaly,
  bl_blink_constant_max, bl_blink_constant_med, bl_blink_constant_low,
  bl_blink_on_off_slow, bl_blink_on_off_fast,
  bl_blink_pattern
};

