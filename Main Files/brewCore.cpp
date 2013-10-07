#include "brewCore.h"
#include "Globals.h"

extern int mode;
extern int potPin;
extern int potValue;
extern int SSRVal;
extern const byte PinElementHlt;

void BrewCoreClass::init()
{
	mode = IDLE;
}

int BrewCoreClass::freeRam(void)
{
	//prints out the ram left on the Arduino --//useful
	extern int  __bss_end;
	extern int  *__brkval;
	int free_memory;
	if((int)__brkval == 0) {
		free_memory = ((int)&free_memory) - ((int)&__bss_end);
	}
	else {
		free_memory = ((int)&free_memory) - ((int)__brkval);
	}
	return free_memory;
}

//
//	This should at some point cause the device to trigger states like, mash in, boil mode, idle... etc.
//
void BrewCoreClass::StateMachine()
{
	switch(mode)
	{
	case IDLE:
		Serial.println("Startup/IDLE waiting commands");
		break;

	case MANUALMODE:
		Serial.println("Manual Mode");
		//manualmode();
		break;

	case AUTOMODE:
		Serial.println("Automatic Mode");
		break;
	}
}

void BrewCoreClass::displayMAC()
{

}

void BrewCoreClass::displayIP()
{

}

BrewCoreClass BREWCOREobj;