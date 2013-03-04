#include "ST7565.h"
#include "stdio.h"
#include <EEPROM.h>
#include "Globals.h"

//
// LETS SEE IF I CAN GET AN ARDUINO TO HELP THE BREW PROCESS.
//

//
//BASICS
//
const byte tempPin	= 0;	
const byte tempPin1 = 1;				// TEMP SENSOR PIN #
const byte PinElementHlt = 10;			// SSR FOR HLT/KETTLE ELEMENT

byte buttonPress = 1;

int incoming = 0;						// placeholder for serial read stuff 


// the LCD back light is connected up to a pin so you can turn it on & off
//#define BACKLIGHT_LED 11
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

const char versionnumber[5] = "0.3";     // current build version

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
//
// the setup routine runs once when you press reset:
//
void setup() {                
	
	pinMode(PinElementHlt, OUTPUT);						// sets the digital pin as output

	Serial.println(freeRam());							// display free ram
  
	glcd.st7565_init();									//Initialize the LCD
	glcd.st7565_command(CMD_DISPLAY_ON);
	glcd.st7565_command(CMD_SET_ALLPTS_NORMAL);
	glcd.st7565_set_brightness(0x18);

	glcd.display();
	delay(2000);
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
	sprintf(tempMsg,"Mash Temp: %d`C", (int)temp);
	glcd.drawstring(0, 1, tempMsg);
	glcd.display();
}


void controlHeating(int temp)
{
	if (temp > HEAT_UP_THRESHOLD && temp <= BOIL_THRESHOLD )// Thresholds can be found in GLOBAL_H				
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
	else if(temp <= LOW
){
		delay(50);
		glcd.drawstring(1, 4, " It is too cold!");
		glcd.display();

	}
	/*else{
		glcd.clear();
	}*/
}


void TempTime()
{
	int temp1 = tempRead(tempPin);
	//int temp2 = tempread(tempPin1);  add me if you have two temp readings ready
	updateHLTDisplay(temp1);				// read the temp from the boil kettle
	//updateMashDisplay1(temp2);

	delay(100);   
	controlHeating(temp1);

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
		Serial.println("IDLE waiting command");
		break;

	case MANUALMODE:
		Serial.println("Manual Mode");
		break;
	}
}


//button1 = Button(2);       // Button 1

byte incomingByte = 0;

void loop() 
{
	manualmode();
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
	//StateMachine();
}