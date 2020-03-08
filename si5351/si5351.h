// A Si5351/A control library that has been modified to be STM32 compatible.
//
// Most of this code come from Hans Summers, 2015 (Website: http://www.hanssummers.com)
//
#ifndef SI5351A_H
#define SI5351A_H

#include <stdint.h>
#include "I2CInterface.h"

void si5351aInit(kc1fsz::I2CInterface* i2c);

void si5351aOutputEnable(bool enabled);

void si5351aSetFrequency(uint32_t frequency, bool reset);

// Setting parameters directly, assuming that the output multisynth is integer
void si5351aSetParameters(uint32_t feedback_mult, uint32_t feedback_num, uint32_t feedback_denom,
	uint8_t output_divider);

#endif //SI5351A_H
