#ifndef _VFOInterface_h
#define _VFOInterface_h

namespace kc1fsz {
	/**
	 * A generic interface for interacting with a VFO
	 */
	class VFOInterface {

	public:
		/**
		 * Used to turn the output on/off
		 */
		virtual void setOutputEnabled(bool e) = 0;

		/**
		 * Used to set the frequency of output.
		 * freqHz - Hertz
		 */
		virtual void setFrequency(double freqHz) = 0;
	};
};

#endif 
