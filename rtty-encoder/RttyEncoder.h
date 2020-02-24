/*
 * RttyEncoder.h
 *
 *  Created on: Feb 22, 2020
 *      Author: Bruce MacKinnon KC1FSZ
 */

#ifndef SRC_RTTYENCODER_H_
#define SRC_RTTYENCODER_H_

#include <stdint.h>
#include "SystemEnv.h"
#include "VFOInterface.h"
#include "Pollable.h"
#include "StatusIndicator.h"

namespace kc1fsz {

	class RttyEncoder : Pollable {

	public:

		RttyEncoder(SystemEnv* sysEnv, VFOInterface* vfo, StatusIndicator* ind);
		virtual ~RttyEncoder();

		/**
		 * Call this function in your processing loop.  Be sure to call it
		 * faster than the symbol rate of your protocol to make sure there
		 * are not gaps.
		 */
		void poll();

		/**
		 * Sets the base frequency for the transmission.
		 */
		void setFreq(unsigned int freqHz);

		/**
		 * Enqueues the text message to be transmitted.  Nothing will actually
		 * be transmitted until start is called.
		 */
		void queueMessage(const char* message);

		void setLoop(bool l);

		/**
		 * Starts the message loop
		 */
		void start();

		/**
		 * Stops the message loop
		 */
		void stop();

		/**
		 * This is the seconds delay between transmissions
		 */
		void setDelay(int delaySeconds);

	private:

		void _startSymbol(unsigned char symbol);
		void _txChar(char c);
		void _txBits(int bits);
		void _queueMark();
		void _queueSpace();
		void _queueSilence();

		SystemEnv* const _sysEnv;
		VFOInterface* const _vfo;
		StatusIndicator* const _ind;

		bool _enabled;
		bool _shiftState;
		unsigned int _baseFreqHz;
		static const unsigned int _outStreamCapacity = 5000;
		unsigned char _outStream[_outStreamCapacity];
		unsigned int _outStreamSize;
		unsigned int _outStreamPtr;
		uint32_t _symbolDurationMs;
		// The time when the current symbol started
		uint32_t _symbolStartMs;
		// Used for managing state transitions
		uint32_t _stateMs;
		// How long we wait between messages
		int _delaySeconds;
		// Controls whether an endless loop is used
		bool _loop;

		enum State {
			IDLE,
			DELAY,
			TRANSMISSION
		};

		State _state;
	};
};

#endif /* SRC_RTTYENCODER_H_ */
