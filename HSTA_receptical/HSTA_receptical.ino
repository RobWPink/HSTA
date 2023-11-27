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
  matrixSetup("HSTA Test Rig", "V0.1.1 | MPa / K");
  delay(500);
  while (!mcp.begin(mcpAddr)) {
    Serial.println("MCP9600 not found.");
    delay(1000);
  }
  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  mcp.setThermocoupleType(MCP9600_TYPE_J);
  mcp.enable(true);
  rtc.init();
  Serial.println("OK");
}


void loop() {
  struct tm myTime;
  rtc.getTime(&myTime); // Read the current time from the RTC into our time structure

  daughterPrint(500);
  if (!timer[0]) { timer[0] = millis(); }
  if (millis() - timer[0] > 100) {
    pt.addValue(fmap(analogRead(PT),0,1023,0,70));
    double temp = mcp.readThermocouple();
    if (-40 <= temp < 500) {
      tt.addValue(temp+273.15);  //Read TT value (K)
    }
    timer[0] = millis();
  }

  currentPressure = pt.getAverage();
  currentTemperature = tt.getAverage();

  if(!timer[1]){timer[1] = millis();}
  if(millis() - timer[1] > 100 && timer[1]){ // IR msg Freq
    timer[1] = millis();
    char data[100]={'\0'};
    char data_[300] = {'\0'};
    char packet[500] = {'\0'};

    for(int i = 0; i < 5; i++){packet[i] = XBOF_sym;}
    packet[5] = BOF_sym;

    strcpy(data,"|ID=SAE␣J2799|VN=01.10|TV=3000.0|RT=H50|FC=");
    strcat(data,digitalRead(SWITCH)?"Stat":"Halt");
    strcat(data,"|MP=");
    strcat(data,formatFloat(currentPressure,5,1));
    strcat(data,"|MT=");
    strcat(data,formatFloat(currentTemperature,5,1));
    strcat(data,"|OD=|");
    uint8_t crc = getCrc(data);
    strcat(data,crc);
    //Serial.println(data);
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
    strcat(packet,EOF_sym);
    for(int i = 0; i < 5; i++){strcat(packet,XBOF_sym);}
    Serial1.write(packet,strlen(packet));
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