#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include "TimerOne.h"

#include "Globals.h"
#include "ST7565.h"
#include "stdio.h"
#include "brewCore.h"

char versionnumber[6]		= "0.7.0";     // current build version
//
// Ethernet
// 
#define ETHERNET_SHIELD 0
#define FAHRENHEIT 0

const int chipSelect		= 4;
#if ETHERNET_SHIELD
byte mac[]					= { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[]					= { 192,168,1, 254 };		//The Arduino device IP address
byte subnet[]				= { 255,255,255,0};
byte gateway[]				= { 192,168,1,1};
IPAddress server(192,168,1,118); 
//uint8_t server[] = {192, 168, 1, 118}; // Local Server
char strURL[70];
bool connected = false;      
#endif
//
//	Classes
//
BrewCoreClass brewCore;
EthernetClient client;

//
// Arduino pins (i/o)
//
//
const byte tempPin			= 2;	// 0 & 1 are off limits for now the Shield fucks them up
const byte tempPin1			= 3;	// TEMP SENSOR PIN #
const byte PinElementHlt	= 10;	// SSR FOR HLT/KETTLE ELEMENT

byte buttonPress			= 1;	// We need a switch or button...

int incoming				= 0;	// placeholder for serial read char variable
byte incomingByte			= 0;

int potPin					= 9;	// select the input pin for the potentiometer
int potValue				= 0;	// variable to store the value coming from the sensor
int SSRVal					= 0;

int errorAlert				= 0;
bool errorSignaled			= false;
int mode;

// the LCD back light is connected up to a pin so you can turn it on & off
//#define BACKLIGHT_LED 11
// Learn how to control via the PMW the different LED's.  IE FLASH, COLOR DEPENDANT ON MODE, ETC.
ST7565 glcd(9, 8, 7, 6, 5);			// WE NEED TO SETUP THE BACKLIGHT AND SET A COLOR.
									// pin 9 - Serial data out (SID)
									// pin 8 - Serial clock out (SCLK)
									// pin 7 - Data/Command select (RS or A0)
									// pin 6 - LCD reset (RST)
									// pin 5 - LCD chip select (CS)

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
extern uint8_t st7565_buffer[1024];

float beerTemperatureActual;
int beerTemperatureSetting = 0;
int fridgeTemperatureActual = 0;
int fridgeTemperatureSetting = 0;

// TIME STUFF
//
extern volatile unsigned long timer0_millis;//used to reset internal timer millis
unsigned long previous_millis_value = 0;	//this stuff for elapsed time
unsigned long cumulativeSeconds		= 0;
unsigned int totalseconds			= 0;
unsigned int totalminutes			= 0;
unsigned int totalhours				= 0;
char total_time[9] = "  :  :  ";
char last_total_time[9] = "  :  :  ";
char stepelapsedtime[9] = "  :  :  ";
unsigned long cyclecount = 0;
char steptotaltime[9] = "  :  :  ";
char temptime[9] = "  :  :  ";

long stepelapsed	= 0;			// number of milliseconds remaining
int stepelapsedsecs = 0;			// number of seconds remaining
int timercalculated = 0;			// has the timer for a timed step been set (initially = false)

int temparray[16] = {0,1,1,2,3,3,4,4,5,6,6,7,8,8,9,9}; // temperature lookup array

#define FIVEMIN (1000UL * 60 * 5)	//* 5 = 5 min
unsigned long rolltime = millis() + FIVEMIN;

void formatTime(int hours, int mins, int secs, char time[]) // format a time to a string from hours, mins, secs
{															// PW 20090203 Added support for overflow of mins and secs
	if (secs>60) {
		secs=secs-60;
		mins++;
	}
	if (mins>60) {
		mins=mins-60;
		hours++;
	}
	time[2]=':';
	time[5]=':';
	time[0]=48+(hours / 10);
	time[1]=48+(hours % 10);
	time[3]=48+(mins / 10);
	time[4]=48+(mins % 10);
	time[6]=48+(secs / 10);
	time[7]=48+(secs % 10);
}

void formatTimeSeconds(long secs, char time[])			// format a time to a string from seconds only
{ 
	int tempsecs = secs % 60;
	int tempmins = (secs / 60) % 60;
	int temphours = secs  / 3600;
	formatTime(temphours,tempmins,tempsecs,time);
}

//
// The setup routine runs once when you press reset or when the device boots up:
//
void setup() 
{
	Serial.begin(9600);									//opens serial port, sets data rate to 9600 bps
// 	Timer1.initialize(2500000);							// initialize timer1, and set a 2,5 second period
// 	Timer1.pwm(8, 512);									// setup pwm on pin 9, duty cycle = 0
// 	Timer1.attachInterrupt(callback);					// attaches callback() as a timer overflow interrupt

	pinMode(53,OUTPUT);									//location of the SS pin, 53 on the Mega, 10 on other
	digitalWrite(53,HIGH);
	pinMode(PinElementHlt, OUTPUT);						// sets the digital pin as output

#if ETHERNET_SHIELD
	Ethernet.begin(mac, ip , gateway , subnet);  
	Serial.println("connecting...");
	if(!client.connect(server, 80)){
		Serial.println("failed.");
	}
	else {
		//tempRead(tempPin);
		client.stop();
	}
#endif

	glcd.st7565_init();									//Initialize the LCD
	glcd.st7565_command(CMD_DISPLAY_ON);
	glcd.st7565_command(CMD_SET_ALLPTS_NORMAL);
	glcd.st7565_set_brightness(0x18);
	glcd.display();

	delay(2000);
	glcd.clear();
	glcd.drawstring(0, 5, versionnumber); 
	delay(1000);
	glcd.clear();

//	brewCore.init();
}

void callback()
{
	digitalWrite(10, digitalRead(10) ^ 1);
}

int led = 13;
int val = 0;        // variable storing value from pot
int burner = 0;
int cycle = 2;     // cycle length of .1 hz
int ssrPin= 10;     // digital pin location of ssr trigger
int ssrAlert=11;

// Reading input from potentiometer
void ssr(byte load){
	int on = (1.0*load/9)*cycle*1200;
	int off = (cycle*1200) - on;
	digitalWrite(led, LOW);
	//digitalWrite(ssrAlert,LOW);
	delay(off);
	digitalWrite(led, HIGH);
	//digitalWrite(ssrAlert,HIGH);
	delay(on);
}

void potAdjustBoil()
{
	//	This will serve as an adjustment function for the boil.  A pot will scale the cycle time for the SSR.
	//  look into this timer for setting the cycle rate for the value of the pot.
	potValue = analogRead(potPin);
	SSRVal = potValue;

	//Timer1.setPwmDuty(PinElementHlt, SSRVal);
	
	Serial.println(SSRVal);

	digitalWrite(led, HIGH);  // turn the ledPin on
	delay(potValue);                  // stop the program for some time
	digitalWrite(led, LOW);   // turn the ledPin off
	delay(potValue);                  // stop the program for some time
	//digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
	//delay(1000);               // wait for a second
	//digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
	//delay(1000);               // wait for a second
	// 	digitalWrite(PinElementHlt, LOW);
	// 	digitalWrite(PinElementHlt, HIGH);
	//delay(1000);
	//return potValue;
}

int tempRead(int tempPinNum)
{
	float tempC;							// TEMP SENSOR
	tempC = analogRead(tempPinNum);			//read the value from the sensor
	tempC = (5.0 * tempC * 100.0)/1024.0;	//convert the analog data to temperature

	//Serial.print("Current Ambient Temp:");
	//Serial.println((int)tempC);           //send the data to the computer
#if FAHRENHEIT
	// now convert to Fahrenheit
	float temperatureF = (tempC * 9.0 / 5.0) + 32.0;
#endif
	//Serial.print(temperatureF); Serial.println(" degrees F");
	//Serial.println(tempC);// this has the format of 21.xx instead of a rounded whole number.
	beerTemperatureActual = tempC;			//sets temp for Python
	serialPrintTemperatures();
	return tempC;
}

void updateHLTDisplay(int const temp)
{
	char tempMsg[32];						//Array to hold our data for the temperature conversions and print it to strings
	sprintf(tempMsg,"HLT  Temp: %d`C", (int)temp);
	glcd.drawstring(0, 0, tempMsg);
	glcd.display();
}

void updateMashDisplay(int const temp)
{
	char tempMsg[32];						//Array to hold our data for the temperature conversions and print it to strings
	//sprintf(tempMsg,"Mash Temp: %d`C", (int)temp);
	glcd.drawstring(0, 1, tempMsg);
	glcd.display();
}

void controlHLTHeating(int temp)
{
	if (temp > HEAT_UP_THRESHOLD && temp <= BOIL_THRESHOLD )	// Thresholds can be found in GLOBAL_H
	{
		glcd.drawstring(0, 2, "     ELEMENT ON!");
		glcd.drawstring(0, 3, "  STARTING TO BOIL!");
		digitalWrite(PinElementHlt, HIGH);
		glcd.display();
	}
	else if (temp > BOIL_THRESHOLD) {
		delay(50);
		glcd.drawstring(0, 2, "      BOILING! ");
		glcd.drawstring(0, 3, "   START SSR FLIP");
		//Start PMW control to force screen to light up as RED LED
		glcd.display();
		if(temp >= BOIL_THRESHOLD)
		{
			//ADD IN POT FUNCTION HERE
			//potAdjustBoil();

// 			if(temp < 27)
// 			{
// 				return;
// 			}
		}
	}
}

void TempTime()  //main function
{
	//int temp1 = 
	tempRead(tempPin);
	
	//serialBeerMessage(1);
	//serialPrintTemperatures();
	//int temp2 = tempRead(tempPin1);			//	add me if you have two temp readings ready
// 	if (errorAlert > 1 && errorSignaled == true)
// 	{
// 		switch (errorAlert)
// 		{
// 		case ERROR:
// 		case ERROR_DB_UPDATE:
// 		case ERROR_TEMP:
// 			errorSignaled = false;
// 			break;
// 			//spit out error to display and pulse a light.
// 			
// 		}
// 		// do something
// 	}
	//updateHLTDisplay(temp1);				//	read the temp from the boil kettle
	//updateMashDisplay1(temp2);
	//controlHLTHeating(temp1);
	
#if ETHERNET_SHIELD
	//
	//	Update the DB
	// 
	if((millis() - rolltime) >= 0)
	{
		Serial.print("update DB: ");
		updateDB(temp1);
		rolltime += FIVEMIN;
	}
#endif
	//
	// Might want to embed this into another function.. timing counter
	//
	while(millis() - previous_millis_value >= 1000)
	{
		cumulativeSeconds++;
		previous_millis_value += 1000;
	}
	formatTimeSeconds(cumulativeSeconds, total_time);

	if (total_time != last_total_time)  // only update if changed to prevent flooding
	{
		last_total_time == total_time;
		//Serial.print("Running time: ");
		delay(700);  //fixme hack to make time ever second
		//Serial.println(total_time);
		glcd.drawstring(0, 7, " RUNTIME:");
		glcd.drawstring(60, 7, total_time);
		glcd.display();
	}
}

void updateDB(int temp)
{
#if ETHERNET_SHIELD
	if (client.connect(server, 80))
	{
		Serial.println("Sending to Server: ");
		sprintf(strURL,"GET /index.php?t=%d",(int)temp);
		client.println(strURL);
		client.println();
		Serial.println(strURL);
		errorAlert = NO_ERROR;
	}
	else {
		Serial.println("failed to connect. Trying again later.");
		errorAlert = ERROR_DB_UPDATE;
	}
	delay(1000);
	client.stop();
#endif
}

void serialPrintTemperatures(void){
	Serial.print(beerTemperatureActual);
	Serial.println(";");
}

void serialBeerMessage(int messageType){
	Serial.print(beerTemperatureActual);

	switch(messageType){
	case BEER_SETTING_FROM_SERIAL:
		Serial.print("\"Beer temperature setting changed to ");
		Serial.print(float(beerTemperatureSetting)); Serial.print("."); Serial.print(float(beerTemperatureSetting));
		Serial.print(" via web interface.\"");
		break;
	case BEER_SETTING_FROM_FRIDGE:
		Serial.print("\"Beer temperature setting changed to ");
		Serial.print(int(beerTemperatureSetting+.5)/10); Serial.print("."); Serial.print(int(beerTemperatureSetting+.5)%10);
		Serial.print(" via fridge menu.\"");
		break;
	case BEER_SETTING_FROM_PROFILE:
		Serial.print("\"Beer temperature setting changed to ");
		Serial.print(int(beerTemperatureSetting+.5)/10); Serial.print("."); Serial.print(int(beerTemperatureSetting+.5)%10);
		Serial.print(" according to temperature profile.\"");
		break;
	default:
		Serial.println("\"Error: Unknown Beer Message Type!\"");
	}

	Serial.println(";");
}

void setupmenu()
{
	Serial.println("RUN: SETUP MENU"); 
	return;
}

void manualmode()
{	
	Serial.println("Starting Manual Mode");
	char wait = 0;					//fix me
	bool state = true;
	while (state)
	{
		TempTime();

		if (Serial.available() > 0)
		{
			wait = Serial.read();
		}
		if (((char)wait == 'x'))	//this will remove you from manual mode. Later a button should control this.
		{
			Serial.println("Leaving manual mode...");
			state = 0;
			return;
		}
	}
}



void loop()
{
	val = analogRead(potPin);    // read value from pot (0-1023)
	burner = ((val*10)/1024);
	Serial.println(val);
	ssr(burner);

	//potAdjustBoil();
// 	for (int i=0; i <= 1023; i += 255)
// 	{
// 		Timer1.setPwmDuty(led, i);
// 		Serial.println(i);
// 
// 		delay(5000);
// 	}
#if ETHERNET_SHIELD
	Ethernet.begin(mac,ip);
#endif
// 	TempTime();
// 
// 	delay(1000);
// 	digitalWrite(53,HIGH);
// 	manualmode();
// 	while (1)
// 	{
// 		if (Serial.available() > 0)
// 		{
// 			incoming = Serial.read();
// 		}
// 		else
// 		{
// 			incoming = 1;
// 		}
// 		//if ((button1) || ((char)incoming == '1'))
// 		if (((char)incoming == '1'))
// 		{
// 			delay(500);
// 			manualmode();
// 		}
// 		if (((char)incoming == '2'))
// 		{
// 			delay(500);
// 			setupmenu();
// 		}
// 	}
}