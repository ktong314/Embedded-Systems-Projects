

#pragma once
#include <stdint.h>

// SW1 is PF4, SW2 is PB1 (not PF0 because I intended to leave SSI1 free in case I needed it for something)
// input: switches = 1 if sw1, 2 if sw2, 3 if both
void sw1_sw2_debounced_init(uint8_t switches);

// SW1 is PF4, SW2 is PB1 (not PF0 because I intended to leave SSI1 free in case I needed it for something)
// input: switches = 1 if sw1, 2 if sw2, 3 if both
void sw1_sw2_level_init(uint8_t switches);

void Timer3A_Handler(void);

void GPIOPortF_Handler(void);