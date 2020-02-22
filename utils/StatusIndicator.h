#ifndef _StatusIndicator_h
#define _StatusIndicator_h

namespace kc1fsz {

	/**
	 * A generic interface for adding a binary status indicator.
	 */
	class StatusIndicator {
	public:

		virtual void setState(bool e) = 0;
	};

};

#endif
