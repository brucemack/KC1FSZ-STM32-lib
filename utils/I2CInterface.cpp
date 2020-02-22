#include "I2CInterface.h"

namespace kc1fsz {

I2CInterface::I2CInterface(I2C_HandleTypeDef* i2c_handle)
:	_i2cHandle(i2c_handle),
	_errorCount(0) {
}

void I2CInterface::write(uint8_t devAddr, uint8_t addr, uint8_t data) {
	// NOTE: PER HAL DOCUMENTATION, ADDRESS NEEDS TO BE SHIFTED LEFT
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(_i2cHandle, (uint16_t)(devAddr << 1), addr, 1, &data, 1, 2);
	if (status != HAL_OK) {
		_errorCount++;
	}
}

uint8_t I2CInterface::read(uint8_t devAddr, uint8_t addr) {
	uint8_t reg_val = 0;
	// TODO: UNDERSTAND THE SHIFT LEFT
	HAL_I2C_Master_Receive(_i2cHandle, (uint16_t)(addr << 1), &reg_val, 1, 2);
	return reg_val;
}

}

