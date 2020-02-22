// A Si5351/A control library that has been modified to be STM32 compatible.
//
// Most of this code come from Hans Summers, 2015 (Website: http://www.hanssummers.com)
//
#ifndef SI5351A_H
#define SI5351A_H

#include <stdint.h>
#include "I2CInterface.h"

void si5351aInit(kc1fsz::I2CInterface* i2c);
void si5351aOutputOff(uint8_t clk);
void si5351aSetFrequency(uint32_t frequency);

#endif //SI5351A_H
