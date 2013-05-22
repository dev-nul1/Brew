// brewCore.h
//
//
#ifndef _BREWCORE_h
#define _BREWCORE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class BrewCoreClass
{
public:

	void init();
	int freeRam(void);

	void potAdjustBoil();
	void StateMachine();
private:
};

#endif
