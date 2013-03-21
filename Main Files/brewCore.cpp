#include "brewCore.h"

void BrewCoreClass::init()
{
}

void BrewCoreClass::test()
{
	Serial.println("BrewCoreClass::test() TEST PRINT!");
	return;
}

int BrewCoreClass::freeRam(void)
{
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
BrewCoreClass BREWCOREobj;