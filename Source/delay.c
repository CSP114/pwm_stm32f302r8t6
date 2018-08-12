#include "delay.h"

void delay_ms(int delay_time){
    for(int i = 0; i < delay_time; i++){
        for(int j = 0; j < 2000; j++){}
    }
}
