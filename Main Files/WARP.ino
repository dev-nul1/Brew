////simple combined thermostat/boil control for Arduino. 
//
//
//void setup(){
//
//int mode = 2;    //read switch on digital pin 2 to detect mode (mash or boil)
//int probe = 5;    //temp probe connected to analog pin 5
//int elements = 3; //element relays connected to digital pin 3
//
//float deadband = 3;     //amount of temperature hysteresis
//float setpoint = 128;    //default setpoint, in raw ADC counts, have to calibrate this
////float setpoint = analogRead(potentiometer)  //or have a convenient temperature setpoint knob
//float reading;             //variable to hold the temperature reading 
//
////do other stuff
//
//}
//
//void loop(){
//
//    if (mode){   //if switch is set to 'mash' mode,  do this 
//
//        float setpoint = analogRead(potentiometer)  //read the setpoint dial
//        reading = analogRead(probe);                    //read in the temperature. Will be 0-255. 
//    
//        if (reading < setpoint - deadband/2){  //it's too cold, turn on the elements
//            digitalWrite(elements, HIGH);
//        } else if (reading >= setpoint + deadband/2){ //it's too hot, turn off the elements
//            digitalWrite(elements, LOW);
//        }
//    
//        delay(1000);  //check once per second
//
//    }
//
//
//    if ( !mode){    //if in 'boil' mode, do this 
//
//         setpoint = analogRead(potentiometer)  //read the setpoint dial to adjust boil vigorosity
//    
//        digitalWrite(elements, HIGH);
//    
//        delay (setpoint *5000 / 255); //stay on 5 seconds if dial is WFO, 0 seconds if it's off 
//    
//        digitalWrite(elements, LOW); 
//    
//        delay(255 - (setpoint *5000 / 255) );   //stay low 5 seconds if dial is off, 0 seconds if it's WFO
//
//        
//
//    }
//
//
//
//}