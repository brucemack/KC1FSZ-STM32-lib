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

		/**
		 * This will only return true for one call per transition.
		 */
		bool isEdge();

		/**
		 * Returns how long we were in the high state
		 */
		unsigned int getHighMs();

	private:

		SystemEnv* const _sysEnv;
		uint32_t _lastTransient;
		bool _transientState;
		bool _stableState;
		bool _isEdge;
		unsigned int _windowMs;
		unsigned int _highStartMs;
	};

};

#endif
