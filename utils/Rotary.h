#ifndef Rotary_h
#define Rotary_h

// Enable this to emit codes twice per step.
// #define HALF_STEP

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Counter-clockwise step.
#define DIR_CCW 0x20

void Rotary_init();
unsigned char Rotary_process();

#endif
