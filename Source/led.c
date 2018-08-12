// Board STM32F302R8
/*  st-nucleo_manual.pdf (User LD2)
    STM32F302R8 -> User LD2 is connected to Pin PB13..
*/

#include "led.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_tim.h"

//Turn on & configure

void led_init(void){
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_GPIO_InitTypeDef mygpio;
  LL_GPIO_StructInit(&mygpio);
  mygpio.Mode = LL_GPIO_MODE_OUTPUT;
  mygpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  mygpio.Pin = LL_GPIO_PIN_13;
  LL_GPIO_Init(GPIOB, &mygpio);
  LL_GPIO_ResetOutputPin (GPIOB, 13);
}

//Use

void led_toggle(void){
  LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_13);
}

void led_write(uint32_t state){
  if(state){
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_13);
  }else{
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_13);
  }
}
