#include "HSTA.h"
//BackOffCapApplication
RunningAverage pt(MOVING_AVG_SIZE);
RunningAverage tt(MOVING_AVG_SIZE);
void (*resetFunc)(void) = 0;  // Allows hard reset of arduino through software
void setup() {
  Wire.begin();
  Wire.setWireTimeout(10000, true);
  pinModeSetup();
  Serial.begin(9600);
  Serial1.begin(38400);
  matrixSetup("HSTA Test Rig", "V0.1.4");
  delay(500);
  rtc.init();
  Serial.println("OK");
}


void loop() {
  struct tm myTime;
  rtc.getTime(&myTime); // Read the current time from the RTC into our time structure

  daughterPrint(500);
  if (!timer[0]) { timer[0] = millis(); }
  if (millis() - timer[0] > 100) {
    pt.addValue(map(analogRead(PT),100,1023,0,13000));
    tt.addValue(JcurveThermistor(analogRead(TT)));  //Read TT value (K)
    timer[0] = millis();
  }

  currentPressure = pt.getAverage();
  currentTemperature = tt.getAverage();

  if(!timer[1]){timer[1] = millis();}
  if(millis() - timer[1] > 100 && timer[1]){ // IR msg Freq
    timer[1] = millis();
    char header[6] = {XBOF_sym,XBOF_sym,XBOF_sym,XBOF_sym,XBOF_sym,BOF_sym};
    char footer[6] = {EOF_sym,XBOF_sym,XBOF_sym,XBOF_sym,XBOF_sym,XBOF_sym};
    char data[100] = {""};
    char data_[150] = {""};
    char packet[200] = {""};
    strcpy(packet,header);
    strcpy(data,"|ID=SAE J2799|VN=01.10|TV=3000.0|RT=H50|FC=");
    strcat(data,digitalRead(SWITCH)?"Dyna":"Halt");
    strcat(data,"|MP=");
    strcat(data,formatFloat(currentPressure/145.0377,5,1));
    strcat(data,"|MT=");
    strcat(data,formatFloat(currentTemperature+273.15,5,1)); //in Kelvin
    strcat(data,"|OD=|");
    uint8_t crc = getCrc(data);
    strcat(data,crc);
    
    int j = 0;
    for(int i = 0; i < strlen(data); i++){
      if(data[i] == XBOF_sym ||
      data[i] == BOF_sym ||
      data[i] == EOF_sym ||
      data[i] == CE_sym){
      
        data_[j++] = CE_sym;
        data_[j++] = (data[i] ^ 0x20);
      }
      else{
        data_[j++] = data[i];
      }
    }
    strcat(packet,data_);
    strcat(packet,footer);
    /*
    for(int i = 0; i < strlen(packet) -1;i++){
      Serial.print(packet[i],HEX);
    }
    Serial.println();
    */
    Serial.println(packet);
    Serial1.write(packet,strlen(packet)-1);
  }
  
  digitalWrite(VALVE,digitalRead(VENT)?HIGH:LOW);

  char logString[32] = {'\0'};
  if(digitalRead(SWITCH)){
    tog = true;
    if(!timer[2]){timer[2] = millis();}
    if(millis() - timer[2] >= 200 && timer[2]){
      char ptStr[10] = {'\0'}; dtostrf(currentPressure,1,0,ptStr);
      char ttStr[10] = {'\0'}; dtostrf(currentTemperature,1,0,ttStr);

      sprintf(logString,"%02d:%02d:%02d,%s,%s\n",myTime.tm_hour, myTime.tm_min, myTime.tm_sec,ptStr,ttStr);
      sendMessage(logString);
      Serial.println(logString);
      memset(logString,'\0',32);
      timer[2] = millis();
    }
  }
  else{
    if(tog){tog = false;sprintf(logString,"\n");}
  }
}