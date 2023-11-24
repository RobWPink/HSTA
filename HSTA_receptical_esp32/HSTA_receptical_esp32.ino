#include "testRig.h"

void setup() {
  Serial.begin(115200);
  while (!Wire.begin(I2C_ADR, I2C_SDA, I2C_SCL, I2C_FRQ)) {
    Serial.println("I2C Begin Failed");
    delay(3000);
  }
  Serial.println("I2C Ready");
  
  while(!SD.begin(8)){
    Serial.println("SD Card Mount Failed");
    delay(3000);
  }
  Serial.println("Mounted SD Card");
  Serial.printf("Available space:  %lluMB / %lluMB\n", SD.totalBytes() / (1024 * 1024) - SD.usedBytes() / (1024 * 1024) - 1, SD.totalBytes() / (1024 * 1024));
  String check = readFile(SD,logFile,1);
  if(check == "fail"){
    Serial.println("log file read error!!");
    writeFile(SD, logFile, headerString);
    delay(100);
    check = readFile(SD,logFile,1);
    if(check != headerString || check == "fail"){Serial.println("log file is STILL read error!!");while(1);}
  }
  else if(check != headerString){
    Serial.println("log file is improperly formatted!!");
    writeFile(SD, logFile, headerString);
    delay(100);
    check = readFile(SD,logFile,1);
    if(check != headerString){Serial.println("log file is STILL improperly formatted!!");while(1);}
  }
  Wire.onReceive(receiveEvent);
}

void loop() {
  if(!timer[3]){timer[3] = millis();}
  if(millis() - timer[3] >= 60000*10 && timer[3]){
    if(fileSize(SD,logFile) > max_filesize){ //if file size becomes greater than 10MB, cycle log file
      if(renameFile(SD,logFile,logFile+"."+String(latestLogNum(SD)))){
        writeFile(SD, logFile, headerString);
      }
    }
    timer[3] = millis();
  }
}
