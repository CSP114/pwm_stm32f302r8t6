// Board STM32F302R8

#include "tim.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_tim.h"
#include "stm32f3xx_ll_gpio.h"
#include "delay.h"

// hz_freq -> PWM Frecuency in Hertz [Hz]
#define tim1_tick 72000000

// Prescaler sets TIM1 to 10[KHz]
#define prescaler (tim1_tick/10000)-1
// TIM1 Ticks at 10[KHz] (0.1ms)
#define set_tick tim1_tick/(prescaler+1)

#define hz_freq 100
#define tim1_period (set_tick/hz_freq)-1
#define dutycicleA (((set_tick/hz_freq)*15)/100)-1
#define dutycicleB (((set_tick/hz_freq)*30)/100)-1
#define dutycicleC (((set_tick/hz_freq)*50)/100)-1

// GPIOA PIN PA8-PA10 PWM initialization
void gpio_pwm_init(void){
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  LL_GPIO_InitTypeDef mygpio;
  LL_GPIO_StructInit (&mygpio);
  mygpio.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
  mygpio.Mode = LL_GPIO_MODE_ALTERNATE;
  mygpio.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
  mygpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  mygpio.Pull = LL_GPIO_PULL_NO;
  mygpio.Alternate = LL_GPIO_AF_6;

  LL_GPIO_Init(GPIOA, &mygpio);
}

// Timer TIM1 Initialization
void tim1_init(void){
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  LL_TIM_InitTypeDef mytimer;
  LL_TIM_StructInit (&mytimer);
  mytimer.CounterMode = LL_TIM_COUNTERMODE_UP;
  mytimer.Prescaler = prescaler;
  mytimer.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  mytimer.Autoreload = tim1_period;
  mytimer.RepetitionCounter = 0;

  LL_TIM_Init(TIM1, &mytimer);  
}

void tim1_start(void){
  LL_TIM_EnableCounter(TIM1);
}

// TIM1 OC configuration
void tim1_pwm_init(void){
  LL_TIM_OC_InitTypeDef mypwm;
  LL_TIM_OC_StructInit (&mypwm);
  mypwm.OCMode = LL_TIM_OCMODE_PWM1;
  mypwm.OCState = LL_TIM_OCSTATE_ENABLE;
  mypwm.OCPolarity = LL_TIM_OCPOLARITY_HIGH;

  mypwm.CompareValue = dutycicleA;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1,&mypwm);
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);
  
  mypwm.CompareValue = dutycicleA;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH2,&mypwm);
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH2);
  
  mypwm.CompareValue = dutycicleA;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH3,&mypwm);
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
}
