#ifndef _StatusIndicator_h
#define _StatusIndicator_h

namespace kc1fsz {

	/**
	 * A generic interface for adding some very simple status indicators:
	 * 1 binary indicator light and a short message.
	 */
	class StatusIndicator {
	public:

		virtual void setLight(bool e) = 0;
		virtual void setMessage(const char* msg) = 0;
	};

};

#endif
