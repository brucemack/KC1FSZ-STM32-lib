/*
 * RttyEncoder.cpp
 *
 *  Created on: Feb 22, 2020
 *      Author: Bruce MacKinnon KC1FSZ
 */
#include "RttyEncoder.h"

static const char shiftLetters = 0x1f;
static const char shiftFigures = 0x1b;
static const int baudotLen = 32;
static const char baudotLetters[] = { 0, 'T', '\r', 'O', ' ', 'H', 'N', 'M', '\n', 'L', 'R', 'G',
  'I', 'P', 'C', 'V', 'E', 'Z', 'D', 'B', 'S', 'Y', 'F', 'X', 'A', 'W', 'J', 0, 'U', 'Q', 'K', 0 };
static const char baudotFigures[] = { 0, '5', '\r', '9', ' ', 0, ',', '.', '\n', ')', '4', '&',
  '8', '0', ':', ';', '3', '\"', '$', '?', 0, '6', '!', '/', '-', '2', '\'', 0, '7', '1', '(', 0 };

// 45.45 baud
static const int baudDelayMs = 22;
// Standard Amateur shift
static const int shiftHz = 170;
static const unsigned char MARK_SYM = 1;
static const unsigned char SPACE_SYM = 2;

namespace kc1fsz {

RttyEncoder::RttyEncoder(SystemEnv* sysEnv, VFOInterface* vfo, StatusIndicator* ind)
:	_sysEnv(sysEnv),
	_vfo(vfo),
	_ind(ind),
	_enabled(false),
	_shiftState(false),
	_outStreamSize(0),
	_outStreamPtr(0),
	_symbolDurationMs(baudDelayMs),
	_stateMs(0),
	_delaySeconds(60),
	_loop(false),
	_state(State::IDLE) {
}

RttyEncoder::~RttyEncoder() {
}

void RttyEncoder::start() {
	_state = State::DELAY;
	_stateMs = _sysEnv->getTimeMs();
	_ind->setMessage("Waiting");
	// Make sure that the VFO is setup
	_vfo->setFrequency((float)_baseFreqHz);
	_vfo->setOutputEnabled(false);
}

void RttyEncoder::stop() {
	_vfo->setOutputEnabled(false);
	_state = State::IDLE;
	_ind->setMessage("Idle");
}

void RttyEncoder::setLoop(bool l) {
	_loop = l;
}


/**
 * This function is called periodically.  It advances through a state
 * machine.
 */
void RttyEncoder::poll() {

	const uint32_t now = _sysEnv->getTimeMs();

	if (_state == State::IDLE) {
		return;
	} else if (_state == State::DELAY) {
		// Finished waiting?
		if (now - _stateMs > (uint32_t)(_delaySeconds * 1000)) {
			_state = State::TRANSMISSION;
			_outStreamPtr = 0;
			_ind->setMessage("Sending");
		}
	} else if (_state == State::TRANSMISSION) {

		if (_outStreamSize == 0) {
			_state = State::DELAY;
			_stateMs = now;
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
				if (_loop) {
					_state = State::DELAY;
					_stateMs = now;
					_ind->setMessage("Waiting");
				} else {
					_state = State::IDLE;
					_ind->setMessage("Done");
				}
			}
		}
	}
}

void RttyEncoder::_startSymbol(unsigned char symbol) {
	// RTTY: Convention: Mark is the upper RF frequency and space is the lower RF frequency.
	// The RTTY "operating frequency" generally coincides with the mark frequency.
	if (symbol == 0) {
		_vfo->setOutputEnabled(false);
		if (_ind)
			_ind->setLight(false);
	} else if (symbol == MARK_SYM) {
		_vfo->setOutputEnabled(true);
		_vfo->setFrequency((float)_baseFreqHz);
		if (_ind)
			_ind->setLight(true);
	} else if (symbol == SPACE_SYM) {
		_vfo->setOutputEnabled(true);
		_vfo->setFrequency((float)(_baseFreqHz - shiftHz));
		if (_ind)
			_ind->setLight(false);
	}
}

void RttyEncoder::setFreq(unsigned int freqHz) {
	_baseFreqHz = freqHz;
}

void RttyEncoder::queueMessage(const char* txt) {
	_outStreamPtr = 0;
	_outStreamSize = 0;
	// Transmit a few mark characters
	_queueMark();
	_queueMark();
	_queueMark();
	// Start the receiver in letter mode
	_txBits(shiftLetters);
	_shiftState = false;
	// Transmit the actual data
	while (*txt != 0) {
		_txChar(*txt);
		txt++;
	}
}

void RttyEncoder::_txChar(char c) {
	// Figure out what character we are dealing with
	int letterIndex = -1;
	int figureIndex = -1;
	for (int i = 0; i < baudotLen; i++) {
		if (letterIndex == -1 && baudotLetters[i] == c) {
			letterIndex = i;
		}
		if (figureIndex == -1 && baudotFigures[i] == c) {
			figureIndex = i;
		}
	}
	if (figureIndex == -1 && letterIndex == -1) {
		// Unrecognized character!  Skip it.
	}
	// Look for case where no shifting is required
	else if (figureIndex == letterIndex) {
		_txBits(figureIndex);
	}
	else if (letterIndex != -1) {
		// Un-shift if needed
		if (_shiftState) {
			_txBits(shiftLetters);
			_shiftState = false;
		}
		_txBits(letterIndex);
	} else if (figureIndex != -1) {
		// Shift if needed
		if (!_shiftState) {
			_txBits(shiftFigures);
			_shiftState = true;
		}
		_txBits(figureIndex);
	}
}

void RttyEncoder::_txBits(int bits) {
	// Start bit
	_queueSpace();
	if (bits & 0x10) _queueMark(); else _queueSpace();
	if (bits & 0x08) _queueMark(); else _queueSpace();
	if (bits & 0x04) _queueMark(); else _queueSpace();
	if (bits & 0x02) _queueMark(); else _queueSpace();
	if (bits & 0x01) _queueMark(); else _queueSpace();
	// Stop bits
	_queueMark();
	_queueMark();
}

void RttyEncoder::_queueMark() {
	if (_outStreamSize < _outStreamCapacity) {
		_outStream[_outStreamSize++] = MARK_SYM;
	}
}

void RttyEncoder::_queueSpace() {
	if (_outStreamSize < _outStreamCapacity) {
		_outStream[_outStreamSize++] = SPACE_SYM;
	}
}

void RttyEncoder::_queueSilence() {
	if (_outStreamSize < _outStreamCapacity) {
		_outStream[_outStreamSize++] = 0;
	}
}

};





