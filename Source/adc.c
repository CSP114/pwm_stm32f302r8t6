// Board STM32F302R8

#include "adc.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_adc.h"
#include "stm32f3xx_ll_dma.h"
#include "delay.h"

//Initialize pins for ADC1 (4 channels <PC0-PC3>)
void adc_config_pins(void){
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_GPIO_InitTypeDef mygpio;
  LL_GPIO_StructInit(&mygpio);
  mygpio.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1| LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
  mygpio.Mode = LL_GPIO_MODE_ANALOG;
  LL_GPIO_Init(GPIOC, &mygpio);
}

//Injected Channels Configuration
void adc_injected_init(int trigger){
  // ADC Synchronous Mode with CLK_DIV/1
  adc_config_pins();
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC1);
  LL_RCC_SetADCClockSource(LL_RCC_ADC1_CLKSRC_HCLK);

  //Common Configuration for ADC Clock
  LL_ADC_CommonInitTypeDef common;
  LL_ADC_CommonStructInit(&common);
  common.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV1;
  LL_ADC_CommonInit(ADC1_COMMON, &common);

  LL_ADC_EnableInternalRegulator(ADC1);
  delay_ms(1);

  //Initialize ADC Calibration
  LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
  while(LL_ADC_IsCalibrationOnGoing(ADC1));

  //Configuration for ADC Capture
  LL_ADC_InitTypeDef myADC;
  LL_ADC_StructInit(&myADC);
  myADC.Resolution = LL_ADC_RESOLUTION_12B;
  myADC.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  LL_ADC_Init(ADC1, &myADC);

  LL_ADC_INJ_SetSequencerDiscont(ADC1, LL_ADC_INJ_SEQ_DISCONT_DISABLE);

  //Injected sequencer configuration
  LL_ADC_ClearFlag_JQOVF(ADC1);
  if(trigger >= 0){
      ADC1->JSQR = (0x1 << 0)// 2 Conversions
      | ( trigger << 2)// External trigger source
      | (0x1 << 6)// Hardware trigger rising edge
      | (0x6 << 8)// JSQ1 = AIN6
      | (0x7 << 14);//// JSQ2 = AIN7
  }
  else{
      ADC1->JSQR = (0x1 << 0) | (0x6 << 8) |  (0x7 << 14);
  }
  
  LL_ADC_ClearFlag_JQOVF(ADC1);

  // Test to check if ADC is Ready to capture
  LL_ADC_Enable(ADC1);
  while(!LL_ADC_IsActiveFlag_ADRDY(ADC1)){};
}

static adc_callback_t adc1_callback;

void adc_injected_enableIRQ(adc_callback_t callback){
    adc1_callback = callback;
    LL_ADC_ClearFlag_JEOS(ADC1);
    LL_ADC_EnableIT_JEOS(ADC1);
    NVIC_EnableIRQ(ADC1_IRQn);
}

void adc_injected_capture(void){
    LL_ADC_ClearFlag_JEOS(ADC1);
    LL_ADC_INJ_StartConversion(ADC1);
}

void adc_injected_waitUntilComplete(void){
    while(!LL_ADC_IsActiveFlag_JEOS(ADC1)){};
    LL_ADC_ClearFlag_JEOS(ADC1);
}

void ADC1_IRQHandler(void){
    LL_ADC_ClearFlag_JEOS(ADC1);
    LL_ADC_ClearFlag_EOS(ADC1);
    adc1_callback();
}

uint16_t adc_injected_read_rank(int rank){
    int ranks[4] = { LL_ADC_INJ_RANK_1, LL_ADC_INJ_RANK_2, LL_ADC_INJ_RANK_3, LL_ADC_INJ_RANK_4 };
    return LL_ADC_INJ_ReadConversionData12(ADC1,ranks[rank]);
}

//Regular channels
void adc_regular_init(void){
    adc_config_pins();
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC1);
    LL_RCC_SetADCClockSource(LL_RCC_ADC1_CLKSRC_HCLK);


    //Common configuration
    LL_ADC_CommonInitTypeDef common;
    LL_ADC_CommonStructInit(&common);
    common.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV1;
    LL_ADC_CommonInit(ADC1_COMMON, &common);


    LL_ADC_EnableInternalRegulator(ADC1);
    delay_ms(1);

    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    while(LL_ADC_IsCalibrationOnGoing(ADC1));

    LL_ADC_InitTypeDef myADC;
    LL_ADC_StructInit(&myADC);
    myADC.Resolution = LL_ADC_RESOLUTION_12B;
    myADC.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    LL_ADC_Init(ADC1, &myADC);

    LL_ADC_REG_InitTypeDef sequencer;
    LL_ADC_REG_StructInit(&sequencer);
    sequencer.TriggerSource = LL_ADC_REG_TRIG_EXT_TIM6_TRGO;
    sequencer.SequencerLength = 3;
    LL_ADC_REG_Init(ADC1, &sequencer);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_6);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_7);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_8);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_4, LL_ADC_CHANNEL_9);
    
    LL_ADC_Enable(ADC1);
    while(!LL_ADC_IsActiveFlag_ADRDY(ADC1)){};  
}

void adc_regular_enableIRQ(adc_callback_t callback){
    adc1_callback = callback;
    LL_ADC_ClearFlag_EOS(ADC1);
    LL_ADC_EnableIT_EOS(ADC1);
    NVIC_EnableIRQ(ADC1_IRQn);
}

void adc_regular_capture(void){
    LL_ADC_ClearFlag_EOS(ADC1);
    LL_ADC_REG_StartConversion(ADC1);
}

uint16_t adc_regular_read(void){
    return LL_ADC_REG_ReadConversionData12(ADC1);
}

static adc_callback_t dma_callback;
uint16_t reloadNData;

void adc_dma_config(uint16_t* dest_address, uint16_t nData, adc_callback_t callback){
    dma_callback = callback;
    reloadNData = nData;
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_DMA_InitTypeDef dma;
    LL_DMA_StructInit(&dma);
    dma.PeriphOrM2MSrcAddress  = (uint32_t) &(ADC1->DR);
    dma.MemoryOrM2MDstAddress  = (uint32_t) dest_address;
    dma.Direction              = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    dma.Mode                   = LL_DMA_MODE_NORMAL;
    dma.PeriphOrM2MSrcIncMode  = LL_DMA_PERIPH_NOINCREMENT;
    dma.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_INCREMENT;
    dma.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    dma.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    dma.NbData                 = nData;
    dma.Priority               = LL_DMA_PRIORITY_HIGH;
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &dma);
    
    LL_ADC_Disable(ADC1);
    
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
     
    LL_ADC_Enable(ADC1);
    while(!LL_ADC_IsActiveFlag_ADRDY(ADC1)){};
    
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);  
}


void DMA1_Channel1_IRQHandler(void){
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_ADC_REG_StopConversion(ADC1);
    dma_callback();
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, reloadNData);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}
