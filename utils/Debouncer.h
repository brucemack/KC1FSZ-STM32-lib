#ifndef _Debouncer_h
#define _Debouncer_h

#include "SystemEnv.h"

namespace kc1fsz {

	/**
	 * A utility for de-bouncing a switch input.
	 */
	class Debouncer {
	public:

		Debouncer(SystemEnv* env, unsigned int windowMs);

		void loadSample(bool sample);

		bool getState();

		bool isEdge();

	private:

		SystemEnv* const _sysEnv;
		uint32_t _lastTransient;
		bool _transientState;
		bool _stableState;
		bool _isEdge;
		unsigned int _windowMs;
	};

};

#endif
