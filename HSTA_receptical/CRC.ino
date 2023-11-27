// CRC parameters (default values are for CCITT 16 Bit):
const int order=16;
const unsigned long polynom=0x1021;
const int direct=1;
const unsigned long crcinit=0x0000;
const unsigned long crcxor=0x00;
const int refin=1;
const int refout=1;

// internal global values:
unsigned long crcmask;
unsigned long crchighbit;
unsigned long crcinit_direct;
unsigned long crcinit_nondirect;
unsigned long crctab[256];

// crc
int i,ii;
unsigned long bit, crc;
unsigned char recchar,*msgdata_ptr;


uint8_t getCrc(unsigned char val[]) {
    crc = crcbitbybitfast(val, msglen(val));
    return crc;
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

void generate_crc_table() {
  // make CRC lookup table used by table algorithms
  int i, j;
  unsigned long bit, crc;
  for (i=0; i<256; i++) {
    crc=(unsigned long)i;
    if (refin) crc=reflect(crc, 8);
    crc<<= order-8;
    for (j=0; j<8; j++) {
      bit=crc & crchighbit;
      crc<<= 1;
      if (bit) crc^= polynom;
    }
    if (refin) crc=reflect(crc, order);
    crc&= crcmask;
    crctab[i]= crc;
  }
}

void crcSetup() {
  // at first, compute constant bit masks for whole CRC and CRC high bit
  crcmask=((((unsigned long)1<<(order-1))-1)<<1)|1;
  crchighbit=(unsigned long)1<<(order-1);

  // generate lookup table
  generate_crc_table();

  if (!direct) {
    crcinit_nondirect=crcinit;
    crc=crcinit;
    for (i=0; i<order; i++) {
      bit=crc & crchighbit;
      crc<<= 1;
      if (bit) crc^= polynom;
    }
    crc&= crcmask;
    crcinit_direct=crc;
  }
  else {
    crcinit_direct=crcinit;
    crc=crcinit;
    for (i=0; i<order; i++) {
      bit=crc & 1;
      if (bit) crc^= polynom;
      crc >>= 1;
      if (bit) crc|= crchighbit;
    }
    crcinit_nondirect=crc;
  }
}


unsigned long crctablefast (unsigned char* p, unsigned long len) {
  // fast lookup table algorithm without augmented zero bytes, e.g., used in pkzip.
  // only usable with polynom orders of 8, 16, 24 or 32.
  unsigned long crc=crcinit_direct;
  if (refin) crc=reflect(crc, order);
  if (!refin) while (len--) crc=(crc << 8) ^ crctab[ ((crc >> (order-8)) & 0xff) ^ *p++];
  else while (len--) crc=(crc >> 8) ^ crctab[ (crc & 0xff) ^ *p++];
  if (refout^refin) crc=reflect(crc, order);
  crc^= crcxor;
  crc&= crcmask;
  return(crc);
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

int msglen(unsigned char * str) {
  int i=0;
  while(*(str++)) i++;
  return i;
}

unsigned long chk_transparency(unsigned int crc_in){
  // transparency check
  unsigned char crc_part1,crc_part2;
  union{
  unsigned long crc_value;
  unsigned char crc_bytes[4];
  }crc;
  bool Transparency=false;
  crc.crc_value=0;
  crc_part1= (unsigned char)((crc_in>>8) & 0xff);
  switch (crc_part1)
  {
  case XBOF_sym:
  case BOF_sym:
  case EOF_sym:
  case CE_sym:
    crc.crc_bytes[3]= CE_sym;
    crc.crc_bytes[2]= (crc_part1^0x20);
    Transparency=true;
    break;
  default:
    crc.crc_bytes[2]= crc_part1;
    crc.crc_bytes[3]= 0;
  }
  crc_part2= (unsigned char)(crc_in & 0xff);
  switch (crc_part2)
  {
  case XBOF_sym:
  case BOF_sym:
  case EOF_sym:
  case CE_sym:
    crc.crc_bytes[1]= CE_sym;
    crc.crc_bytes[0]= (crc_part2^0x20);
    Transparency=true;
    break;
  default:
    crc.crc_bytes[0]= crc_part2;
    crc.crc_bytes[1]= 0;
  }
  if (Transparency) return crc.crc_value;
  else return crc.crc_bytes[0] | (crc.crc_bytes[2]<<8);
}
