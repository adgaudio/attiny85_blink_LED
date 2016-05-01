#ifndef __BL_BLINK_H__
#define __BL_BLINK_H__

#include <util/delay.h>

/* Some blink routines */
#define bl_blink_led(ms, off_ms) {\
  OCR0A = 255;\
  _delay_ms(ms);\
  OCR0A = 0;\
  _delay_ms(off_ms);\
}

void bl_blink_constant_max();
void bl_blink_constant_med();
void bl_blink_constant_low();
void bl_blink_on_off_slow();
void bl_blink_on_off_fast();

void bl_blink_smooth();
void bl_blink_ismooth();
void bl_blink_sos();
void bl_blink_pattern();
void bl_blink_anomaly();


/* Array of all blink routines */
typedef void (*bl_blink_routine)();
extern const bl_blink_routine bl_blink_all_routines[];
extern const int bl_blink_num_routines;

#endif /* __BL_BLINK_H__ */
