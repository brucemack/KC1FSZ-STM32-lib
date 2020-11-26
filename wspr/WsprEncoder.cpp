#include "WsprEncoder.h"
//#include "wspr2.h"
#include <stdio.h>

// http://www.arrl.org/files/file/QEX_Next_Issue/May-June2019/Steber.pdf

// 1.4648 baud
static const int baudDelayMs = 682;

/*
0 0.0 Hz
1 1.4648 Hz
2 2.9296 Hz
3 4.3944 Hz
*/
// The symbol offsets for the 4-FSK protocol
static const double symbolFreqs[] = { 0.0000, 1.4648, 2.9296, 4.3944 };

namespace kc1fsz {

WsprEncoder::WsprEncoder(SystemEnv* sysEnv, VFOInterface* vfo, StatusIndicator* ind)
:	_sysEnv(sysEnv),
	_vfo(vfo),
	_ind(ind),
	_enabled(false),
	_outStreamSize(0),
	_outStreamPtr(0),
	_symbolDurationMs(baudDelayMs),
	_stateMs(sysEnv->getTimeMs()),
	_cycleSeconds(120),
	_lastCycleSecondDisplayed(0),
	_state(State::IDLE) {
}

void WsprEncoder::start() {
	_startTransmission();
}

void WsprEncoder::stop() {
	_vfo->setOutputEnabled(false);
	_state = State::IDLE;
}

void WsprEncoder::_startTransmission() {
	_state = State::TRANSMISSION;
	_outStreamPtr = 0;
	// Reset the cycle timer
	_stateMs = _sysEnv->getTimeMs();
	// Power on the VFO
	_vfo->setOutputEnabled(true);
}

/**
 * This function is called periodically.  It advances through a state
 * machine.
 */
void WsprEncoder::poll() {

	const uint32_t now = _sysEnv->getTimeMs();
	const int cycleSecond = (now - _stateMs) / 1000;

	// Update the display
	if (cycleSecond != _lastCycleSecondDisplayed) {
		char buf[16];
		if (_state == State::TRANSMISSION) {
			sprintf(buf,"%3d TX",cycleSecond);
		} else {
			sprintf(buf,"%3d   ",cycleSecond);
		}
		_ind->setMessage(buf);
		_lastCycleSecondDisplayed = cycleSecond;
	}

	if (_state == State::IDLE) {
		return;
	} else if (_state == State::DELAY) {
		// Finished waiting?
		if (now - _stateMs > (uint32_t)(_cycleSeconds * 1000)) {
			_startTransmission();
		}
	} else if (_state == State::TRANSMISSION) {

		if (_outStreamSize == 0) {
			_state = State::DELAY;
			return;
		}

		uint32_t dur =  now - _symbolStartMs;

		// Ready to move to the next symbol?
		if (_outStreamPtr == 0 || dur >= _symbolDurationMs) {
			// Start sounding the symbol
			_symbolStartMs = now;
			_startSymbol(_outStream[_outStreamPtr]);
			// Advance to the next symbol
			_outStreamPtr++;
			// End of message?
			if (_outStreamPtr == _outStreamSize) {
				// Shut off the carrier
				_vfo->setOutputEnabled(false);
				_state = State::DELAY;
			}
		}
	}
}

void WsprEncoder::_startSymbol(unsigned char symbol) {
	_vfo->setFrequency((double)_baseFreqHz + symbolFreqs[(int)symbol]);
}

void WsprEncoder::setFreq(unsigned int freqHz) {
	_baseFreqHz = freqHz;
}

void WsprEncoder::setParameters(const char* call, const char* grid, int power) {
	//encodeWsprMessage(call, grid, power, _outStream);
	//encodeWsprMessage(call, grid, power, _outStream);
	//wspr_encode(call, grid, 7, _outStream);

	_outStreamSize = 162;
}

};





