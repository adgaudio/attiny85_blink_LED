#include "bl_debounce.h"
#include "bl_blink.h"
#include "bl_power.h"
#include <avr/interrupt.h>
#include <avr/io.h>


uint8_t bl_debounce_is_pressed() {
  /*
   * Debounce algorithm to determine if button state
   *
   * Returns:
   * 0 if button not pressed recently or in a fuzzy state
   * 1 if end of a press (button-up)
   * 2 if start of a press (button-down)
   * 3 if button-down state sustained
   */
  // static uint16_t state = 0xe000; // Current debounce status.  static preserves var across calls
  static uint64_t state = 0x0000000000000000; // Current debounce status.  static preserves var across calls
  state = (state<<1) | !(!(PINB & _BV(PB2)));  // add 1 if pin reads HIGH
  if (state == 0xfffff00000000000) return 1;  // button-up press
  if (state == 0x00000000000fffff) return 2;  // button-down press
  if (state == 0xffffffffffffffff) return 3;
  return 0;
}


ISR(TIMER0_OVF_vect) {
  /* Interrupt routine that runs periodically, or whenever timer0 overflows
   * This routine detects whether a button has been pressed and switches modes
   * or powers down the device
   */
  uint8_t press = bl_debounce_is_pressed();
  static uint16_t long_press_cnt = 0;

  switch (press) {
    case 1:  // button-up press
      if (long_press_cnt > 200) {
        bl_routine = (bl_routine + 1) % bl_blink_num_routines;
      } else if (long_press_cnt > 0) {
        bl_routine = (bl_routine + (bl_blink_num_routines - 1)) % bl_blink_num_routines;
      }
      long_press_cnt = 0;
      break;
    case 2:  // button-down press
      long_press_cnt = 1;
      break;
    case 3:  // button held down
      long_press_cnt += 1;
      if (long_press_cnt > 1000) {
        bl_blink_smooth();
        long_press_cnt = 0;
        bl_power_down();
      }
      break;
  }
}
