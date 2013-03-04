// Setup Menu for user defined variables 
//
// ATmega328P has 1024 bytes of EEPROM, values are from 0-1023
// 
//  0 = testmode   (DEFAULT 0, SET TO 1 TO ENABLE SKIP STEP) When testmode is enabled you can skip thru steps in AutoMode by pressing button 2
//  1 = countdowntimer (DEFAULT 0, SET BETWEEN 0-255 FOR DELAY AT START OF AUTOMODE)
//  2 = checkfloat (DEFAULT 1, SET TO 0 IF YOU DON'T REQUIRE CHECKING OF THE FLOAT SWITCH)
//  3 = striketemp  (DEFAULT 0, OTHERWISE MANUALLY SET)
//  4 = targetmashtemp (DEFAULT 63-69) 
//  5 = fudgefactor (DEFAULT 5)
//  6 = graintowaterratio (DEFAULT 3)   
//  7 = spargetemp (DEFAULT 98)
//  8 = doughinpumpruntime (DEFAULT 9) ***
//  9 = mashlength (DEFAULT 60)
// 10 = mashoutpumpruntime (DEFAULT 6) ***
// 11 = mashoutstirtime (DEFAULT 5)
// 12 = mashoutresttime (DEFAULT 10)a
// 13 = vorlaufpumpruntime (DEFAULT 10) ***
// 14 = fillkettlepumpruntime (DEFAULT 11) ***
// 15 = boiltemp (DEFAULT 99)
// 16 = boillength (DEFAULT 90)
// 17 = hopaddition1 (DEFAULT 60)
// 18 = hopaddition2 (DEFAULT 15)
// 19 = hopaddition3 (DEFAULT 5)
// 20 = endofboilresttime (DEFAULT 5)


void setupmenu()
{
	Serial.println("OK HERE I AM AT SETUP");
	return;
}



byte checkfloat;

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


void manualmode(){
	checkfloat  = EEPROM.read(200);
	Serial.print(checkfloat, DEC); // print value on screen
	
	Serial.println("Starting Manual Mode");
	 while (1) 
	{
	TempTime();
	}
}