#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdlib.h>
#include <stdint.h>


#define BAD_TEMP -32768
#define FAHRENHEIT 0
#define OLED 0
//
//	For Python
// 
enum beerMessages{
	BEER_SETTING_FROM_FRIDGE,
	BEER_SETTING_FROM_SERIAL,
	BEER_SETTING_FROM_PROFILE
};

enum fridgeMessages{
	FRIDGE_SETTING_FROM_FRIDGE,
	FRIDGE_SETTING_FROM_SERIAL,
	FRIDGE_DOOR_OPEN,
	FRIDGE_DOOR_CLOSED,
	POSPEAK,
	NEGPEAK,
	POSDRIFT,
	NEGDRIFT,
	ARDUINO_START
};

//Timers
typedef enum {
	TIMER_MASH,
	TIMER_BOIL,
	TIMER_NUMTIMERS
} timers;

//Error
 typedef enum {
	 NO_ERROR = 0,
	 ERROR = 0x0001,
	 ERROR_DB_UPDATE,
	 ERROR_TEMP,
	 ERROR_ALERT
}errorList;

//
//Brew Steps PS
//
const float BOIL_THRESHOLD = 99.0f;
const float HEAT_UP_THRESHOLD = 10.0f;
const float	LOW_THRESHOLD = 19.0f;

// typedef enum  {
// 	BOIL_THRESHOLD = 30,
// 	HEAT_UP_THRESHOLD = 10,
// 	LOW_THRESHOLD = 19
// }thresholds;

//
//Brew Steps PS
//
typedef enum {
	STARTUP = 0,
	IDLE,
	MANUALMODE,
	AUTOMODE,
}brewMode;

//Brew Steps
typedef enum {
	BREWSTEP_FILL = 0x1000,
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
//#define CMD_MSG_FIELDS 25
//#define CMD_FIELD_CHARS 21

//#define ASCII		0
//#define BTNIC		1
//#define BINARY	2

//typedef enum {
//	CONTROLSTATE_OFF,
//	CONTROLSTATE_AUTO,
//	CONTROLSTATE_ON,
//	NUM_CONTROLSTATES
//} ControlState;

#endif