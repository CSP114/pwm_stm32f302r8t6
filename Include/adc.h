#ifndef ADC_H_
#define ADC_H_
  
  #include <stdint.h>
  
  typedef void (*adc_callback_t)(void);
  
  // Pins Initialization
  void adc_config_pins(void);
  
  // Injected Sequence Functions
  void adc_injected_init(int trigger);
  void adc_injected_enableIRQ(adc_callback_t callback);
  void adc_injected_capture(void);
  uint16_t adc_injected_read_rank(int rank);
  
  // Regular Sequence Functions
  void adc_regular_init(void);
  void adc_regular_enableIRQ(adc_callback_t callback);
  void adc_regular_capture(void);
  uint16_t adc_regular_read(void);
  
  void adc_dma_config(uint16_t* dest_address, uint16_t nData, adc_callback_t callback);

#endif
