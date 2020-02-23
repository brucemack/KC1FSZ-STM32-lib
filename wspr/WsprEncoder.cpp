#include "WsprEncoder.h"
#include "wspr.h"

// 1.4648 baud
static const int baudDelayMs = 682;
// The symbols for the 4-FSK protocol (x100)
static const int symbolFreqs[] = { 0, 146, 293, 439 };

/*
0 0.0 Hz
1 1.4648 Hz
2 2.9296 Hz
3 4.3944 Hz
*/

namespace kc1fsz {

WsprEncoder::WsprEncoder(SystemEnv* sysEnv, VFOInterface* vfo, StatusIndicator* ind)
:	_sysEnv(sysEnv),
	_vfo(vfo),
	_ind(ind),
	_outStreamSize(0),
	_outStreamPtr(0),
	_symbolDurationMs(baudDelayMs),
	_stateMs(0),
	_cycleSeconds(120),
	_state(State::IDLE) {
}

void WsprEncoder::start() {
	_state = State::TRANSMISSION;
	// Synchronize to this point
	_stateMs = _sysEnv->getTimeMs();
	_ind->setMessage("Sending");
}

void WsprEncoder::stop() {
	_vfo->setOutputEnabled(false);
	_state = State::IDLE;
	_ind->setMessage("Idle");
}

/**
 * This function is called periodically.  It advances through a state
 * machine.
 */
void WsprEncoder::poll() {

	const uint32_t now = _sysEnv->getTimeMs();

	if (_state == State::IDLE) {
		return;
	} else if (_state == State::DELAY) {
		// Finished waiting?
		if (now - _stateMs > (uint32_t)(_cycleSeconds * 1000)) {
			_state = State::TRANSMISSION;
			_outStreamPtr = 0;
			_ind->setMessage("Sending");
		}
	} else if (_state == State::TRANSMISSION) {

		if (_outStreamSize == 0) {
			_state = State::DELAY;
			_ind->setMessage("Waiting");
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
				_ind->setMessage("Waiting");
			}
		}
	}
}

void WsprEncoder::_startSymbol(unsigned char symbol) {
	_vfo->setOutputEnabled(true);
	_vfo->setFrequency(_baseFreqHz + symbolFreqs[(int)symbol]);
}

void WsprEncoder::setFreq(unsigned int freqHz) {
	_baseFreqHz = freqHz;
}

void WsprEncoder::setParameters(const char* call, const char* grid, int power) {
	encodeWsprMessage(call, grid, power, _outStream);
	_outStreamSize = 162;
}

};





