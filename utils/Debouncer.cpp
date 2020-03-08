#include "Debouncer.h"

namespace kc1fsz {

Debouncer::Debouncer(SystemEnv* env, unsigned int windowMs)
:	_sysEnv(env),
	_lastTransient(0),
	_transientState(false),
	_stableState(false),
	_isEdge(false),
	_windowMs(windowMs) {

}

void Debouncer::loadSample(bool sample) {
	if (sample != _transientState) {
		_lastTransient = _sysEnv->getTimeMs();
		_transientState = sample;
	} else if (_sysEnv->getTimeMs() - _lastTransient > _windowMs) {
		if (_transientState != _stableState) {
			_stableState = _transientState;
			_isEdge = true;
		}
	}
}

bool Debouncer::getState() {
	return _stableState;
}

bool Debouncer::isEdge() {
	bool r = _isEdge;
	_isEdge = false;
	return r;
}

}

