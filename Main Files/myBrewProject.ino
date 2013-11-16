#include <SPI.h>

#include <EEPROM.h>

#include "Globals.h"
#include "ST7565.h"
#include "stdio.h"
#include "brewCore.h"

char versionnumber[6]		= "0.8.5";     // current build version

//
//	Classes
//
BrewCoreClass brewCore;
//
// Arduino pins (i/o)
//
//
const byte tempPin			= A2;	// 0 & 1 are off limits for now the Shield fucks them up
//const byte tempPin1			= 3;	// TEMP SENSOR PIN #
int potPin			= 10;	// select the input pin for the potentiometer
const int SSRpinControlHLT  = 12;	// SSR FOR HLT/KETTLE ELEMENT

const int led				= 13;

//Potentiometer variables
int val						= 0;	// variable storing value from pot
int burner					= 0;
int cycle					= 2;	// cycle length of .1 hertz

byte buttonPress			= 1;	// We need a switch or button...  DO WE NEED THIS?

//HOLDERS
int incoming				= 0;	// placeholder for serial read char variable
byte incomingByte			= 0;

int potValue				= 0;	// variable to store the value coming from the sensor
int SSRVal					= 0;

int errorAlert				= 0;
bool errorSignaled			= false;

//const int chipSelect		= 4;

int mode;

// the LCD back light is connected up to a pin so you can turn it on & off
#define BACKLIGHT_LED_RED 4
#define BACKLIGHT_LED_GREEN 3
//#define BACKLIGHT_LED_BLUE 3

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
extern volatile unsigned long timer0_millis;	//used to reset internal timer millis
unsigned long previous_millis_value = 0;		//this stuff for elapsed time
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
	if (secs>60) 
	{
		secs=secs-60;
		mins++;
	}
	if (mins>60) 
	{
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
#if OLED
const int displayDC = 5;       // OLED display data control
const int displayReset = 6;    // OLED display reset 
const int outputEnable = 7;    // Logic translator output enable (active HIGH)
const int displaySelect = 10;  // OLED display SPI slave select
#endif

//
// The setup routine runs once when you press reset or when the device boots up:
//
void setup() 
{
	Serial.begin(9600);									//opens serial port, sets data rate to 9600 bps
#if OLED
	digitalWrite(displaySelect,HIGH);
	pinMode(displaySelect, OUTPUT);
	pinMode(displayReset, OUTPUT);
	pinMode(displayDC, OUTPUT);

	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV8);
	SPI.setDataMode(SPI_MODE3);
	SPI.begin();
	analogReference(DEFAULT);
	OLED_Init(); 
	clear_Screen();
#endif
	//pinMode(BACKLIGHT_LED_GREEN, OUTPUT);
	pinMode(BACKLIGHT_LED_RED, OUTPUT);
	pinMode(SSRpinControlHLT, OUTPUT);					// sets digital pin as an output for SSR
	pinMode(led, OUTPUT);

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

	//brewCore.init();
}

// the follow variables is a long because the time, measured in milliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)
long previousMillis = 0;        // will store last time LED was updated
int ledState = LOW;             // ledState used to set the LED
// Reading input from potentiometer
void ssr(byte load)
{
	int on = (1.0*load/9)*cycle*1200;
	int off = (cycle*1200) - on;

	unsigned long currentMillis = millis();

	if(currentMillis - previousMillis > on) 
	{
		// save the last time you blinked the LED 
		previousMillis = currentMillis;   

		// if the LED is off turn it on and vice-versa:
		if (ledState == LOW)
			ledState = HIGH;
		else
			ledState = LOW;

		// set the LED with the ledState of the variable:
		digitalWrite(led, ledState);
		digitalWrite(SSRpinControlHLT, ledState);
	}
}

float tempRead(int tempPinNum)
{
	float tempC;							// TEMP SENSOR
	tempC = analogRead(tempPinNum);			//read the value from the sensor
	tempC = (5.0 * tempC * 100.0)/1024.0;	//convert the analog data to temperature
#if FAHRENHEIT  // now convert to Fahrenheit
	float temperatureF = (tempC * 9.0 / 5.0) + 32.0;
	Serial.print(temperatureF); Serial.println(" degrees F");
#endif
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

void animateBacklight(int mode) 
{
	int brightness = 0;
	int fadeAmount = 5;

	if (mode == 1)
	{
		while (brightness <= 255) 
		{
			//analogWrite(BACKLIGHT_LED_RED, brightness);
			//analogWrite(BACKLIGHT_LED_GREEN, brightness);
			analogWrite(BACKLIGHT_LED_RED, brightness);
			brightness += fadeAmount;
			delay(20);
		}	
	}
	
}

void controlHLTHeating(float temp)
{
	if (temp <= BOIL_THRESHOLD )	// Thresholds can be found in GLOBAL_H temp > HEAT_UP_THRESHOLD &&
	{
		glcd.drawstring(0, 2, "ELEMENT ON!");
		glcd.drawstring(0, 3, "HLT HEATING");
		digitalWrite(SSRpinControlHLT, HIGH);
		digitalWrite(led, HIGH);
		animateBacklight(1);
		glcd.display();
	}
	else if (temp > BOIL_THRESHOLD)
	{
		delay(50);
		glcd.drawstring(0, 2, "      BOILING! ");
		glcd.drawstring(0, 3, "   START SSR FLIP");
		//Start PMW control to force screen to light up as RED LED
		glcd.display();
		animateBacklight(1);
		if(temp >= BOIL_THRESHOLD)
		{
			val = analogRead(potPin);    // read value from pot (0-1023)
			burner = ((val*10)/1024);
			ssr(burner);
			return;
		}
		return;
	}
	return;
}

void TempTime()  //main function
{
	tempRead(tempPin);
	controlHLTHeating(beerTemperatureActual);
	//serialPrintTemperatures();
	handleSerialCommunication();
	if (errorAlert > 1 && errorSignaled == true)
	{
		switch (errorAlert)
		{
		case ERROR:
		case ERROR_DB_UPDATE:
		case ERROR_TEMP:
			errorSignaled = false;
			break;
			//spit out error to display and pulse a light.
		}
	}	
	//
	// Might want to embed this into another function.. timing counter
	//
	while(millis() - previous_millis_value >= 1000)
	{
		cumulativeSeconds++;
		previous_millis_value += 1000;
	}
	formatTimeSeconds(cumulativeSeconds, total_time);

	if (total_time != last_total_time)			// only update if changed to prevent flooding
	{
		last_total_time == total_time;
		//Serial.print("Running time: ");
		delay(700);								//fixme hack to make time ever second
		Serial.println(total_time);
		glcd.drawstring(0, 7, " RUNTIME:");
		glcd.drawstring(60, 7, total_time);
		glcd.display();
	}
}

void serialPrintTemperatures(void)
{
	Serial.print(beerTemperatureActual);
	Serial.println(";");
}

void serialBeerMessage(int messageType)
{
	Serial.print(beerTemperatureActual);

	switch(messageType)
	{
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

void handleSerialCommunication(void)
{

	if (Serial.available() > 0)
	{
		char inByte = Serial.read();
		switch(inByte)
		{
		case 'r': //Data request
			serialPrintTemperatures();
			break;

		case 'b': //Set to constant beer temperature
			pinMode(BACKLIGHT_LED_GREEN,OUTPUT);
			digitalWrite(BACKLIGHT_LED_GREEN, LOW);
			
			break;

		case 'p': //Set profile temperature
			beerTemperatureSetting = numberFromSerial();
			break;

		case 'f': //Set to constant fridge temperature
// 			while (brightness <= 255) {
				//analogWrite(BACKLIGHT_LED_RED, brightness);
				//analogWrite(BACKLIGHT_LED_GREEN, brightness);
				analogWrite(BACKLIGHT_LED_RED, 128);
				//brightness += fadeAmount;
				delay(20);
			//}
			break;
		case 'd':
			{
				digitalWrite(BACKLIGHT_LED_GREEN, HIGH);
				digitalWrite(BACKLIGHT_LED_RED, HIGH);
				//digitalWrite(BACKLIGHT_LED_BLUE, HIGH);
				break;
			}
		case 's': //Settings requested
			switch(mode)
			{
			}
			break;

		case 'l': //LCD contents requested
			pinMode(BACKLIGHT_LED_RED, OUTPUT);
			digitalWrite(BACKLIGHT_LED_RED, LOW);
			break;
		default:
			Serial.println(".Invalid command Received by Arduino");
		}
		Serial.flush();
	}
}

int numberFromSerial(void)
{
	char numberString[8];
	unsigned char index=0;
	delay(10);
	while(Serial.available() > 0)
	{
		delay(10);
		numberString[index++]=Serial.read();
		if(index>6)
		{
			break;
		}
	}
	numberString[index]=0;
	return atoi(numberString);
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
	manualmode();

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
// 		if (((char)incoming == '2'))
// 		{
// 			delay(500);
// 			manualmode();
// 		}
// 		if (((char)incoming == '3'))
// 		{
// 			delay(500);
// 			setupmenu();
// 		}
// 	}
}