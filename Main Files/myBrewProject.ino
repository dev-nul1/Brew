#include "ST7565.h"
#include "stdio.h"

// LETS SEE IF I CAN GET AN ARDUINO TO HELP THE BREW PROCESS.



const byte tempPin	= 0;	
const byte tempPin1 = 1;				// TEMP SENSOR PIN #
const byte PinElementHlt = 10;			// SSR FOR HLT/KETTLE ELEMENT





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
// the setup routine runs once when you press reset:


const char versionnumber[5] = ".1";     // current build version

void setup() {                
	
	pinMode(PinElementHlt, OUTPUT);		// sets the digital pin as output

	Serial.println(freeRam());			// display free ram
  
	glcd.st7565_init();									//Initialize the LCD
	glcd.st7565_command(CMD_DISPLAY_ON);
	glcd.st7565_command(CMD_SET_ALLPTS_NORMAL);
	glcd.st7565_set_brightness(0x18);

	glcd.display();
	delay(2000);
	glcd.clear();

	Serial.begin(9600);					//opens serial port, sets data rate to 9600 bps
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
		glcd.drawstring(0, 3, "   TRYING TO BOIL!");
		
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


void loop() 
{
	//Serial.println(freeRam());
	int temp1 = tempread(tempPin);
	//int temp2 = tempread(tempPin1);
	updateHLTDisplay(temp1);				// read the temp from the boil kettle
	//updateMashDisplay1(temp2);

	delay(100);   
	controlHeating(temp1);


}