float fmap(float x, float a, float b, float c, float d){
  float f=x/(b-a)*(d-c)+c;
  return f;
}

unsigned long reflect (unsigned long crc, int bitnum) {
  // reflects the lower 'bitnum' bits of 'crc'
  unsigned long i, j=1, crcout=0;
  for (i=(unsigned long)1<<(bitnum-1); i; i>>=1) {
    if (crc & i) crcout|=j;
    j<<= 1;
  }
  return (crcout);
}

unsigned long crcbitbybitfast(unsigned char* p, unsigned long len) {
  // fast bit by bit algorithm without augmented zero bytes.
  // does not use lookup table, suited for polynom orders between 1...32.
  unsigned long i, j, c, bit;
  unsigned long crc=crcinit_direct;
  for (i=0; i<len; i++) {
    c=(unsigned long)*p++;
    if (refin) c=reflect(c, 8);
    for (j=0x80; j; j>>=1) {
      bit=crc & crchighbit;
      crc<<= 1;
      if (c & j) bit^= crchighbit;
      if (bit) crc^= polynom;
    }
  }
  if (refout) crc=reflect(crc, order);
  crc^= crcxor;
  crc&= crcmask;
  return(crc);
}

int msglen(unsigned char * str)
{
int i=0;
while(*(str++))
i++;
return i;
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
    bigMatrix[0].writeInt(currentPressure);  // Write local pressure values to 7-seg screen
    smallMatrix[0].squareDisplay(0);
    bigMatrix[1].writeInt(currentTemperature);
    smallMatrix[1].squareDisplay(0);
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
