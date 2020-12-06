#ifndef _WsprEncoder_h
#define _WsprEncoder_h

#include <stdint.h>
#include "utils/SystemEnv.h"
#include "utils/VFOInterface.h"
#include "utils/Pollable.h"
#include "utils/StatusIndicator.h"

namespace kc1fsz {

	class WsprEncoder : Pollable {

	public:

		WsprEncoder(SystemEnv* sysEnv, VFOInterface* vfo, StatusIndicator* ind);

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
		 * Sets the offset frequency for the transmission.
		 */
		void setOffsetFreq(unsigned int offsetFreqHz);

		/**
		 * Sets the callsign, location, and power
		 */
		void setParameters(const char* call, const char* grid, int powerDbm);

		/**
		 * Starts the message loop
		 */
		void start();

		/**
		 * Stops the message loop
		 */
		void stop();

	private:

		void _startSymbol(unsigned char symbol);
		void _startTransmission();


		SystemEnv* const _sysEnv;
		VFOInterface* const _vfo;
		StatusIndicator* const _ind;

		bool _enabled;
		// For 40m: 7038.600
		unsigned int _baseFreqHz;
		// Generally around 1,500 Hz
		unsigned int _offsetFreqHz;
		static const unsigned int _outStreamCapacity = 162;
		unsigned char _outStream[_outStreamCapacity];
		unsigned int _outStreamSize;
		unsigned int _outStreamPtr;
		uint32_t _symbolDurationMs;
		// The time when the current symbol started
		uint32_t _symbolStartMs;
		// Used for managing state transitions
		uint32_t _stateMs;
		int _cycleSeconds;
		int _lastCycleSecondDisplayed;

		enum State {
			IDLE,
			DELAY,
			TRANSMISSION
		};

		State _state;
	};
};

#endif
