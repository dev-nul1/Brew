#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Mega (ATmega1280)
#define __AVR_ATmega1280__
#define 
#define ARDUINO 101
#define ARDUINO_MAIN
#define __AVR__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

void formatTime(int hours, int mins, int secs, char time[]);
void formatTimeSeconds(long secs, char time[]);
//
void ssr(byte load);
void potAdjustBoil();
int tempRead(int tempPinNum);
void updateHLTDisplay(int const temp);
void updateMashDisplay(int const temp);
void controlHLTHeating(int temp);
void TempTime();
void serialPrintTemperatures(void);
void serialBeerMessage(int messageType);
void setupmenu();
void manualmode();
//

#include "D:\Arduino\arduino-1.0.1\hardware\arduino\variants\mega\pins_arduino.h" 
#include "D:\Arduino\arduino-1.0.1\hardware\arduino\cores\arduino\arduino.h"
#include "D:\CODING\Arduino\Brew\Main Files\myBrewProject.ino"
#include "D:\CODING\Arduino\Brew\Main Files\Globals.h"
#include "D:\CODING\Arduino\Brew\Main Files\brewCore.cpp"
#include "D:\CODING\Arduino\Brew\Main Files\brewCore.h"
#endif
