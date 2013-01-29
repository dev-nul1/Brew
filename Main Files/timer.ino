//
//unsigned long buzzerCycleStart = millis(); //last time the alarm went on
//
//byte lastEEPROMWrite[2];
//
//void setTimer(byte timer, int minutes) {
//  if (minutes != -1) {
//    //A cancel action as not been selected!
//    timerValue[timer] = minutes * 60000;
//    lastTime[timer] = millis();
//    timerStatus[timer] = 1;
//    setTimerStatus(timer, 1);
//    setTimerRecovery(timer, minutes);
//  }
//}
//
//void pauseTimer(byte timer) {
//  if (timerStatus[timer]) {
//    //Pause
//    timerStatus[timer] = 0;
//  } else {
//    //Unpause
//    timerStatus[timer] = 1;
//    lastTime[timer] = millis();
//  }
//  setTimerStatus(timer, timerStatus[timer]);
//}
//
//void clearTimer(byte timer) {
//  timerValue[timer] = 0;
//  timerStatus[timer] = 0;
//  setTimerStatus(timer, 0);
//  setTimerRecovery(timer, 0);
//}
//
//void updateTimers() {
//  for (byte timer = TIMER_MASH; timer <= TIMER_BOIL; timer++) {
//    if (timerStatus[timer]) {
//      if (estop) pauseTimer(timer);
//      unsigned long now = millis();
//      if (timerValue[timer] > now - lastTime[timer]) {
//        timerValue[timer] -= now - lastTime[timer];
//      } else {
//        #ifdef DEBUG_TIMERALARM
//          logStart_P(LOGDEBUG);
//          if(timer == TIMER_MASH) logField("MASH_TIMER has expired"); else logField("BOIL_TIMER has expired");
//          logEnd();
//        #endif
//        timerValue[timer] = 0;
//        timerStatus[timer] = 0;
//        setTimerStatus(timer, 0);
//        setTimerRecovery(timer, 0);  // KM - Moved this from below to be event driven
//        setAlarm(1);
//      }
//      lastTime[timer] = now;
//    }
//
//    byte timerHours = timerValue[timer] / 3600000;
//    byte timerMins = (timerValue[timer] - timerHours * 3600000) / 60000;
//
//    //Update EEPROM once per minute
//    if (timerMins != lastEEPROMWrite[timer]) {
//      lastEEPROMWrite[timer] = timerMins;
//      if (timerValue[timer]) setTimerRecovery(timer, timerValue[timer]/60000 + 1);
//    }
//  }
//}
//
////This function allows modulation of buzzer when the alarm is on.
//void updateBuzzer() {
//  //Retreive the status of the alarm. (Removed by Matt. This value is always in memory)
//  //byte alarmStatus = bitRead(EEPROM.read(306), 2);
//  //Set the buzzer according the user custom buzzer modulation
//  setBuzzer(alarmStatus);
//}
//
//void setAlarm(boolean alarmON) {
//  setAlarmStatus(alarmON);
//  setBuzzer(alarmON);  
//}
//
////This function allow to modulate the sound of the buzzer when the alarm is ON.
////The modulation varies according the custom parameters.
////The modulation occurs when the buzzerCycleTime value is larger than the buzzerOnDuration
//void setBuzzer(boolean alarmON) {
//  if (alarmON) {
//    #ifdef BUZZER_CYCLE_TIME
//      //Alarm status is ON, Buzzer will go ON or OFF based on modulation.
//      //The buzzer go OFF for every moment passed in the OFF window (low duty cycle).
//      unsigned long now = millis(); //What time is it? :-))      
//     
//      //Now, by elimation, identify scenarios where the buzzer will go off.
//      if (now < buzzerCycleStart + BUZZER_CYCLE_TIME) {
//        //At this moment ("now"), the buzzer is in the OFF window (low duty cycle).
//        if (now > buzzerCycleStart + BUZZER_ON_TIME) {
//          //At this moment ("now"), the buzzer is NOT within the ON window (duty cycle) allowed inside the buzzer cycle window.
//          //Set or keep the buzzer off
//          alarmPin.set(0);
//        }
//      } else {
//        //The buzzer go ON for every moment where buzzerCycleStart < "now" < buzzerCycleStart + buzzerOnDuration
//        alarmPin.set(1); //Set the buzzer On
//        buzzerCycleStart = now; //Set a new reference time for the begining of the buzzer cycle.
//      }
//    #else
//      alarmPin.set(1); //Set the buzzer On
//    #endif
//  } else {
//    //Alarm status is OFF, Buzzer goes Off
//    alarmPin.set(0);
//  }
//}
