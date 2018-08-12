#include "stm32f3xx.h"                  // Device header
#include "delay.h"
#include "led.h"
#include "button.h"
#include "adc.h"
#include "tim.h"

int main(){
  //int button_value, counter;
  led_init();
  button_init();
  gpio_pwm_init();
  tim1_init();
  tim1_start();
  tim1_pwm_init();
  
  while(1){
   
  }
  
}
