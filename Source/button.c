// Board STM32F302R8
/*  st-nucleo_manual.pdf (User B1)
    STM32F302R8 -> User B1 is connected to Pin PC13..
*/

#include "button.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_gpio.h"

#define BUTTON_PIN LL_GPIO_PIN_13
#define BUTTON_PORT GPIOC

//Turn on & configure
void button_init(void){
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_GPIO_InitTypeDef mygpio;
  LL_GPIO_StructInit (&mygpio);
  mygpio.Mode = LL_GPIO_MODE_INPUT;
  mygpio.Pin = BUTTON_PIN;
  mygpio.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(BUTTON_PORT, &mygpio);
}

int button_read(void){
  return LL_GPIO_IsInputPinSet(BUTTON_PORT, BUTTON_PIN);
}
