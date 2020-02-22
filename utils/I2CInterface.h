#ifndef _I2CInterface_h
#define _I2CInterface_h

#include <stdint.h>

extern "C" {
	#include "stm32f1xx_hal.h"
};

namespace kc1fsz {

	class I2CInterface {
	public:

		I2CInterface(I2C_HandleTypeDef* i2c_handle);

		void write(uint8_t devAddr, uint8_t addr, uint8_t data);
		uint8_t read(uint8_t devAddr, uint8_t addr);

	private:

		I2C_HandleTypeDef* _i2cHandle;
		unsigned int _errorCount;
	};
};

#endif
