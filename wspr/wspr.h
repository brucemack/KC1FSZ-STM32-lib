#ifndef _wspr_h
#define _wspr_h

extern "C" {
	void encodeWsprMessage(const char *call, const char *grid, const int power, unsigned char* symbols);
}

#endif
