#ifndef LED_H_
#define LED_H_

  #include <stdint.h>

  void led_init(void);
  void led_toggle(void);
  void led_write(uint32_t state);

#endif
