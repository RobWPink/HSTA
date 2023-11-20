#include "HSTA.h"debouncer
RunningAverage bank1RA(MOVING_AVG_SIZE);

void (*resetFunc)(void) = 0;  // Allows hard reset of arduino through software
void setup() {
  Wire.begin();
  Wire.setWireTimeout(10000, true);
  pinModeSetup();
  Serial.begin(9600);
  matrixSetup("HSTA Test Rig", "V0.0.2");

  while (!mcp.begin(mcpAddr)) {
    Serial.println("MCP9600 not found.");
    delay(1000);
  }
  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  mcp.setThermocoupleType(MCP9600_TYPE_K);
  mcp.enable(true);

}


void loop() {
  struct tm myTime;
  rtc.getTime(&myTime); // Read the current time from the RTC into our time structure

  if (digitalRead(BUTTON)) {if(!debouncer){ debouncer = millis(); }}
  else{debouncer = 0;}
  if (millis() - debouncer > 150 && debouncer){
    if(!button){ tog = !tog; }
    button = true;
  }
  else{button = false;}

  if(tog){
    digitalWrite(VALVE,HIGH);
  }
  else{
    digitalWrite(VALVE,LOW);
  }
}