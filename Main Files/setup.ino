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



