#include "string.h"
#include "utils/SerialDriver.h"

#define SEND_TIMEOUT_MS 5

SerialDriver::SerialDriver(UART_HandleTypeDef* huart)
:	_huart(huart),
	_eomDelimPtr(0),
	_eomDelimSize(0),
	_messageBufferPtr(0) {
}

void SerialDriver::start() {
	// Schedule the first receive
	HAL_UART_Receive_IT(_huart, &_receiveBuffer, 1);
}


void SerialDriver::setMessageHandler(MessageHandler* mh) {
	_messageHandler = mh;
}

// TODO: ERROR CHECK ON MAX SIZE
void SerialDriver::setEOM(const char* delim) {
	strcpy(_eomDelim, delim);
	_eomDelimSize = strlen(delim);
	_eomDelimPtr = 0;
}

void SerialDriver::onReceive(UART_HandleTypeDef* huart) {
	_onReceive(_receiveBuffer);
	// Schedule another receive
	HAL_UART_Receive_IT(_huart, &_receiveBuffer, 1);
}

int SerialDriver::send(const char* message) {
	if (HAL_UART_Transmit(_huart, (uint8_t*)message, strlen(message), SEND_TIMEOUT_MS) == HAL_OK) {
		return 0;
	} else {
		return -1;
	}
}

void SerialDriver::_onReceive(uint8_t data) {
	// Save received data
	_messageBuffer[_messageBufferPtr++] = (char)_receiveBuffer;
	// Figure out if a complete message has been received
	if ((char)data == _eomDelim[_eomDelimPtr]) {
		// Still successful, move forward
		_eomDelimPtr++;
		// Complete delimiter comparison?
		if (_eomDelimPtr == _eomDelimSize) {
			_eomDelimPtr = 0;
			// Null-terminate the message
			_messageBuffer[_messageBufferPtr - _eomDelimSize] = 0;
			// Fire the callback
			if (_messageHandler != 0) {
				_messageHandler->onMessage(_messageBuffer);
			}
			// Reset to the start
			_messageBufferPtr = 0;
		}
	} else {
		// Compare failed reset to the beginning of the delimiter
		_eomDelimPtr = 0;
	}
}
