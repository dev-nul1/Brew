#include "brewCore.h"
extern int tester;

void BrewCoreClass::init()
{
}

void BrewCoreClass::test()
{
	Serial.println("BrewCoreClass::test() TEST PRINT!");
	Serial.println("Choose:");
	Serial.println(tester);
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


void BrewCoreClass::potAdjustBoil()
{
	//
	//	This will serve as an adjustment function for the boil.  A pot will scale the cycle time for the SSR.
	// #include "TimerOne.h"
	// look into this timer for setting the cycle rate for the value of the pot.
	// 

// 	potValue = analogRead(potPin); // read the value from the pot
// 	Serial.println( potValue );
// 	delay(1);
// 
// 	return potValue;

// 	potValue = analogRead(potPin);
// 	if (potValue >= 1010) {
// 		SSRVal = 1024;
// 	}
// 
// 	else {
// 		SSRVal = potValue;
// 	}
// 
// 
// 	Timer1.setPwmDuty(9, SSRVal);
// 	Serial.println(SSRVal);        

	//return potValue;
}

BrewCoreClass BREWCOREobj;