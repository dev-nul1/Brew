#include "ST7565.h"
#include "stdio.h"
#include <EEPROM.h>

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


// the LCD backlight is connected up to a pin so you can turn it on & off
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

const char versionnumber[5] = "0.2";     // current build version

byte checkfloat;
// TIME STUFF
//
//extern volatile unsigned long timer0_millis;  //used to reset internal timer millis
unsigned long previous_millis_value = 0;		//this stuff for elapsed time
unsigned long cumulativeSeconds = 0;
unsigned int totalseconds = 0;
unsigned int totalminutes = 0;
unsigned int totalhours = 0;
char totaltime[9] = "  :  :  ";
char lasttotaltime[9] = "  :  :  ";
char stepelapsedtime[9] = "  :  :  ";
unsigned long cyclecount=0;
char steptotaltime[9] = "  :  :  ";
char temptime[9] = "  :  :  ";


long stepelapsed = 0;  // number of milliseconds remaining
int stepelapsedsecs = 0;  // number of seconds reamaining
int timercalculated = 0; // has the timer for a timed step been set (initially = false)

int temparray[16] = {
  0,1,1,2,3,3,4,4,5,6,6,7,8,8,9,9}; // temperature lookup array


 void formattime(int hours, int mins, int secs, char time[]) { // format a time to a string from hours, mins, secs
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


void formattimeseconds(long secs, char time[])			// format a time to a string from seconds only
{		
  int tempsecs = secs % 60;
  int tempmins = (secs / 60) % 60;
  int temphours = secs  / 3600;
  formattime(temphours,tempmins,tempsecs,time);

}


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


int freeRam(void)
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


int tempread(int tempPinNum)
{
	float tempC;							// TEMP SENSOR 
  	tempC = analogRead(tempPinNum);			//read the value from the sensor
	tempC = (5.0 * tempC * 100.0)/1024.0;	//convert the analog data to temperature

	//sprintf(tempMsg,"Current Temp: %d", (int)tempC);

	Serial.print("Current Ambient Temp:");
	Serial.println((int)tempC);             //send the data to the computer
											// now convert to Fahrenheight
											//float temperatureF = (tempC * 9.0 / 5.0) + 32.0;
											//Serial.print(temperatureF); Serial.println(" degrees F");
	return tempC;
}


void updateHLTDisplay(int const temp)
{
	char tempMsg[32];						//Array to hold our data for the tempature conversions and print it to strings
	sprintf(tempMsg,"HLT  Temp: %d`C", (int)temp);
	glcd.drawstring(0, 0, tempMsg);
	glcd.display();
}


void updateMashDisplay(int const temp)
{
	char tempMsg[32];						//Array to hold our data for the tempature conversions and print it to strings
	sprintf(tempMsg,"Mash Temp: %d`C", (int)temp);
	glcd.drawstring(0, 1, tempMsg);
	glcd.display();
}


void controlHeating(int temp)
{
	if (temp > 20 && temp <= 24) {
		glcd.drawstring(0, 2, "     ELEMENT ON!");
		glcd.drawstring(0, 3, "  STARTING TO BOIL!");
		
		digitalWrite(PinElementHlt, HIGH); 
		glcd.display();
	}
	else if (temp > 25) {
		delay(50);
	    glcd.drawstring(1, 5, " It is too hot!");
	    glcd.display(); 
		digitalWrite(PinElementHlt, LOW); 
	}
	else if(temp <=19){
		delay(50);
	   glcd.drawstring(1, 5, " It is too cold!");
	   glcd.display();
	}
	/*else{
		glcd.clear();
	}*/
}


void TempTime()
{
	int temp1 = tempread(tempPin);
	//int temp2 = tempread(tempPin1);
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
    formattimeseconds(cumulativeSeconds,totaltime);

    if (totaltime!=lasttotaltime)  // only update if changed to prevent flooding
    {
      Serial.print("Running time: ");              
      delay(300);
      Serial.println(totaltime);
      lasttotaltime==totaltime;
    }
}


//
//	This should at some point cause the device to trigger states like, mash in, boil mode, idle... etc.
//

void StateMachine()
{
	int ok;
	switch(ok)
	{
	case 0:
		Serial.println("state waiting command");
		break;

	case 1:
		Serial.println("state boilMode");
		break;
	}
}


//button1 = Button(2);       // Button 1

byte incomingByte = 0;

void loop() 
{
	while (1) 
	{
		if (Serial.available() > 0)
		{
		  incoming = Serial.read();
		}
		else
		{
		  incoming = 0;
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

	//TempTime();
}