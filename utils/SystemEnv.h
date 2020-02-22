#ifndef _SystemEnv_h
#define _SystemEnv_h

#include <stdint.h>

namespace kc1fsz {

	class SystemEnv {
	public:

		virtual uint32_t getTimeMs() = 0;
	};
};

#endif
