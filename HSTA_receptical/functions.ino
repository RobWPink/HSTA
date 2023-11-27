float fmap(float x, float a, float b, float c, float d){
  float f=x/(b-a)*(d-c)+c;
  return f;
}

void sendMessage(char* msg){
  Wire.beginTransmission(0x03);
  Wire.write(msg); 
  Wire.endTransmission(); //message buffer is sent with Wire.endTransmission()                   
}

void daughterPrint(unsigned long inter){
  if (!daughterTimer) { daughterTimer = millis(); }
  if (millis() - daughterTimer > inter && daughterTimer) {
    daughterTimer = millis();
    bigMatrix[0].writeInt(currentPressure*145.03773773);  // Write local pressure values to 7-seg screen
    smallMatrix[0].squareDisplay(digitalRead(VENT)?100:0);
    bigMatrix[1].writeInt(currentTemperature);
    smallMatrix[1].squareDisplay(digitalRead(SWITCH)?100:0);
    smallMatrix[2].squareDisplay(0);
  }
}

void matrixSetup(String s1, String s2) {
  //begin matrix
  for (int i = 0; i < 3; i++) {
    smallMatrix[i].begin();
    bigMatrix[i].begin();
  }
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
  lcd.setBacklight(HIGH);
  lcd.display();
}

char *formatFloat(float number, int len, int prec) {
  dtostrf(number,len,prec,buffer);
  for (int i = 0; i < strlen(buffer); i++){
    if (buffer[i]==' '){buffer[i]='0';}
    else {break;}
  }
  return buffer;
}

void pinModeSetup() {
  pinMode(SWITCH, INPUT);
  pinMode(VENT, INPUT);
  pinMode(VALVE, OUTPUT);
  digitalWrite(VALVE, LOW);

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(SS, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(SS, LOW);
  digitalWrite(CS, HIGH);
  
  pinMode(REDE, OUTPUT);
  digitalWrite(REDE,HIGH);

  pinMode(ESTOP_BREAK, OUTPUT);
  digitalWrite(ESTOP_BREAK, HIGH);

  pinMode(LED_PWR, OUTPUT);
  digitalWrite(LED_PWR, HIGH);

  pinMode(TRACO_24VDC, OUTPUT);
  digitalWrite(TRACO_24VDC, HIGH);
}
