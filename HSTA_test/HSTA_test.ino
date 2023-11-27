#include "HSTA.h"

void (*resetFunc)(void) = 0;  // Allows hard reset of arduino through software
void setup() {
  Wire.begin();
  Wire.setWireTimeout(10000, true);
  pinModeSetup();
  Serial.begin(9600);
  Serial1.begin(38400);
  matrixSetup("HSTA Test Rig", "V0.1.1");
  crcSetup();
}


void loop() {

  currentPressure = 35 + random(0,4);
  currentTemperature = 273.15 + random(25,30);

  
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
    strcat(data,0?"Dyna":"Halt");
    strcat(data,"|MP=");
    strcat(data,formatFloat(currentPressure,5,1));
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
    Serial1.write(packet,strlen(packet)-1);
  }
  
}