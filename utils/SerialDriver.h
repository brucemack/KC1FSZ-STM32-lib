/*
 * SerialDriver.h
 *
 *  Created on: Dec 6, 2020
 *      Author: bruce
 */

#ifndef INC_SERIALDRIVER_H_
#define INC_SERIALDRIVER_H_

#if defined(STM32F4)
extern "C" {
	#include "stm32f4xx_hal.h"
};
#endif

#include "MessageHandler.h"

class SerialDriver {
public:

	SerialDriver(UART_HandleTypeDef* huart);

	void start();

	void setMessageHandler(MessageHandler* mh);

	// Sends the end-of-message delimiter
	void setEOM(const char* delim);

	void onReceive(UART_HandleTypeDef* huart);

	// Synchronous send. The message must be null-terminated.
	// Returns on success, -1 on failure
	int send(const char* message);

private:

	void _onReceive(uint8_t data);

	// The handle to the UART itself
	UART_HandleTypeDef* _huart;
	// Where the asynchronous activity writes to
	uint8_t _receiveBuffer;

	// This is what we are searching for
	char _eomDelim[16];
	unsigned int _eomDelimPtr;
	unsigned int _eomDelimSize;

	// Used for assembling messages
	char _messageBuffer[1024];
	unsigned int  _messageBufferPtr;

	// Called when a complete message is received
	MessageHandler* _messageHandler;
};

#endif /* INC_SERIALDRIVER_H_ */
