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