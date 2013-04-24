#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

#include "Globals.h"
#include "ST7565.h"
#include "stdio.h"
#include "brewCore.h"
//#include "TimerOne.h"   //CHECK THE INFO RELATED TO THIS TIMER.  GET THE POT WIRED UP.
//
// LETS SEE IF I CAN GET AN ARDUINO TO HELP THE BREW PROCESS.
// ISSUES WITH VAX??? http://forums.wholetomato.com/forum/topic.asp?TOPIC_ID=11091
char versionnumber[5] = "0.6";     // current build version

byte checkfloat = 0;
bool state = false;

int tester = 5;
//
// Ethernet
// 
const int chipSelect = 4;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192,168,1, 254 };		//The Arduino device IP address
byte subnet[] = { 255,255,255,0};
byte gateway[] = { 192,168,1,1};
IPAddress server(192,168,1,118); 
//uint8_t server[] = {192, 168, 1, 118}; // Local Server
char strURL[70];

//
//	Classes
//
BrewCoreClass brewCore;
EthernetClient client;//(server, 80);

//Client client(server, 80);
bool connected = false;      

//Client client(server, 80);

//
// Arduino pins (i/o)
//
// 0 & 1 are off limits for now
const byte tempPin	= 2;
const byte tempPin1 = 3;				// TEMP SENSOR PIN #
const byte PinElementHlt = 10;			// SSR FOR HLT/KETTLE ELEMENT

byte buttonPress = 1;					// We need a switch or button...

int incoming = 0;						// placeholder for serial read stuff

int potPin	=	3;						// select the input pin for the potentiometer
int potValue =	0;						// variable to store the value coming from the sensor
int SSRVal = 0;


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
// the buffer for the image display
extern uint8_t st7565_buffer[1024];


// TIME STUFF
//
//extern volatile unsigned long timer0_millis;  //used to reset internal timer millis
unsigned long previous_millis_value = 0;		//this stuff for elapsed time
unsigned long cumulativeSeconds = 0;
unsigned int totalseconds = 0;
unsigned int totalminutes = 0;
unsigned int totalhours = 0;
char total_time[9] = "  :  :  ";
char last_total_time[9] = "  :  :  ";
char stepelapsedtime[9] = "  :  :  ";
unsigned long cyclecount = 0;
char steptotaltime[9] = "  :  :  ";
char temptime[9] = "  :  :  ";

long stepelapsed = 0;		// number of milliseconds remaining
int stepelapsedsecs = 0;	// number of seconds remaining
int timercalculated = 0;	// has the timer for a timed step been set (initially = false)

int temparray[16] = {
	0,1,1,2,3,3,4,4,5,6,6,7,8,8,9,9}; // temperature lookup array

void formatTime(int hours, int mins, int secs, char time[]) { // format a time to a string from hours, mins, secs
	// PW 20090203 Added support for overflow of mins and secs
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
// the setup routine runs once when you press reset:
//
//
void setup() {

	pinMode(53,OUTPUT); //location of the SS pin, 53 on the Mega, 10 on other
	digitalWrite(53,HIGH);
	Ethernet.begin(mac, ip , gateway , subnet);  
	Serial.println("connecting...");
	if(!client.connect(server, 80)){
		Serial.println("failed.");
	}
	else {
		//tempRead(tempPin);
		client.stop();
	}

	pinMode(PinElementHlt, OUTPUT);						// sets the digital pin as output

	//Serial.println(freeRam());						// display free ram

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

	Serial.begin(9600);									//opens serial port, sets data rate to 9600 bps
}


int tempRead(int tempPinNum)
{
	float tempC;							// TEMP SENSOR
	tempC = analogRead(tempPinNum);			//read the value from the sensor
	tempC = (5.0 * tempC * 100.0)/1024.0;	//convert the analog data to temperature

	//sprintf(tempMsg,"Current Temp: %d", (int)tempC);

	Serial.print("Current Ambient Temp:");
	Serial.println((int)tempC);             //send the data to the computer
	// now convert to Fahrenheit
	//float temperatureF = (tempC * 9.0 / 5.0) + 32.0;
	//Serial.print(temperatureF); Serial.println(" degrees F");
	Serial.println(tempC);
	return tempC;
}

void updateBKDisplay(int const temp)
{
	char tempMsg[32];						//Array to hold our data for the temperature conversions and print it to strings
	sprintf(tempMsg,"HLT  Temp: %d`C", (int)temp);
	glcd.drawstring(0, 0, tempMsg);
	glcd.display();
}

void updateMashDisplay(int const temp)
{
	char tempMsg[32];						//Array to hold our data for the temperature conversions and print it to strings
	sprintf(tempMsg,"Mash Temp: %d`C", (int)temp);
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
			digitalWrite(PinElementHlt, LOW);
			delay(1000);
			digitalWrite(PinElementHlt, HIGH);
			delay(1000);
			if(temp < 27)
			{
				return;
			}
		}
	}
	else if(temp <= LOW)
	{
		delay(50);
		glcd.drawstring(1, 4, " It is too cold!");
		glcd.display();
	}
}

void TempTime()  //main function
{
	int temp1 = tempRead(tempPin);
	//int temp2 = tempread(tempPin1);  add me if you have two temp readings ready
	updateBKDisplay(temp1);				// read the temp from the boil kettle
	//updateMashDisplay1(temp2);

	//delay(100);
	controlHLTHeating(temp1);
	
	Serial.print("update DB: ");
	updateDB(temp1);
	//
	// Might want to embed this into another function..
	//
	while(millis() - previous_millis_value >= 1000)
	{
		cumulativeSeconds++;
		previous_millis_value += 1000;
	}
	formatTimeSeconds(cumulativeSeconds,total_time);
	if (total_time!=last_total_time)  // only update if changed to prevent flooding
	{
		Serial.print("Running time: ");
		delay(300);
		Serial.println(total_time);
		last_total_time==total_time;
		glcd.drawstring(0, 7, " RUNTIME:");
		glcd.drawstring(60, 7, total_time);
		glcd.display();
	}
}

void updateDB(int temp)
{
	if (client.connect(server, 80))
	{
		Serial.println("Sending to Server: ");
		sprintf(strURL,"GET /index.php?t=%d",(int)temp);
		client.println(strURL);
		client.println();
		Serial.println(strURL);
	}
	else {
		Serial.println("failed to connect. Trying again later.");
	}
	delay(1000);
	client.stop();

}

void setupmenu()
{
	Serial.println("RUN: SETUP MENU");
	return;
}

void manualmode(){
	//checkfloat = EEPROM.read(200);
	//Serial.print(checkfloat, DEC); // print value on screen

	
	Serial.println("Starting Manual Mode");
	state = true;
	while (state)
	{
		TempTime();

		char wait;					//fix me
		if (Serial.available() > 0)
		{
			wait = Serial.read();
		}
		if (((char)wait == 'x'))
		{
			Serial.println("Punting");
			state = 0;
			return;
		}
	}
}

//
//	This should at some point cause the device to trigger states like, mash in, boil mode, idle... etc.
//
void StateMachine()
{
	int ok;
	ok = true;
	switch(ok == true)
	{
	case IDLE:
		Serial.println("IDLE waiting commands");
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

//button1 = Button(2);       // Button 1

byte incomingByte = 0;

void loop()
{
	Ethernet.begin(mac,ip);
	digitalWrite(53,HIGH);
// 	if (client.connect(server, 80))
// 	{
// 		tempRead(tempPin1);
// 		Serial.println("Sending to Server: ");
// 	}
// 	else {
// 		Serial.println("failed to connect. Trying again later.");
// 	}
// 	delay(1000);
// 	client.stop();
	
	//brewCore.test();
	while (1)
	{
		if (Serial.available() > 0)
		{
			incoming = Serial.read();
		}
		else
		{
			incoming = 1;
		}
		//if ((button1) || ((char)incoming == '1'))
		if (((char)incoming == '1'))
		{
			delay(500);
			manualmode();
		}
		if (((char)incoming == '2'))
		{
			delay(500);
			setupmenu();
		}
	}
}