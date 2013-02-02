void manualmode(){
	checkfloat  = EEPROM.read(2);
	Serial.print(checkfloat, DEC); // print value on screen
	
	Serial.println("Starting Manual Mode");
	 while (1) 
	{
	TempTime();
	}
}