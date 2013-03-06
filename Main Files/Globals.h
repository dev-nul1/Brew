#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdlib.h>
#include <stdint.h>
#define BAD_TEMP -32768

//Timers
typedef enum {
	TIMER_MASH,
	TIMER_BOIL,
	TIMER_NUMTIMERS
} timers;


//
//Brew Steps PS 
// 
typedef enum  {
	BOIL_THRESHOLD = 27,
	HEAT_UP_THRESHOLD = 20,
	LOW_THRESHOLD = 19
}thresholds;


//
//Brew Steps PS 
// 
typedef enum  {
	IDLE,
	MANUALMODE
}brewSteps;

//Brew Steps
typedef enum {
	BREWSTEP_FILL,
	BREWSTEP_DELAY,
	BREWSTEP_PREHEAT,
	BREWSTEP_ADDGRAIN,
	BREWSTEP_REFILL,
	BREWSTEP_DOUGHIN,
	BREWSTEP_ACID,
	BREWSTEP_PROTEIN,
	BREWSTEP_SACCH,
	BREWSTEP_SACCH2,
	BREWSTEP_MASHOUT,
	BREWSTEP_MASHHOLD,
	BREWSTEP_SPARGE,
	//Reserve additional steps for batch sparge logic
	//BREWSTEP_BOIL = BREWSTEP_SPARGE + SPARGE_BATCHESMAX * 3 + 1,
	BREWSTEP_CHILL,
	BREWSTEP_STEPCOUNT
} brewStepsOLD;


//Events
typedef enum {
	EVENT_STEPINIT,
	EVENT_STEPEXIT,
	EVENT_SETPOINT,
	EVENT_ESTOP,
	EVENT_COUNT
} events;

//Log Constants
#define CMD_MSG_FIELDS 25
#define CMD_FIELD_CHARS 21

#define ASCII	0
#define BTNIC	1
#define BINARY	2

//typedef enum {
//	CONTROLSTATE_OFF,
//	CONTROLSTATE_AUTO,
//	CONTROLSTATE_ON,
//	NUM_CONTROLSTATES
//} ControlState;

#endif