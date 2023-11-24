//#include "stdafx.h"
#include <string.h>
#include <stdio.h>

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

// subroutines
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

int msglen(unsigned char * str){
  int i=0;
  while(*(str++))
  i++;
  return i;
}

unsigned long _tmain(unsigned char* msgptr){
  unsigned long bit, crc;
  // at first, compute constant bit masks for whole CRC and CRC high bit
  crcmask=((((unsigned long)1<<(order-1))-1)<<1)|1;
  crchighbit=(unsigned long)1<<(order-1);
  // generate lookup table
  generate_crc_table();

  // compute missing initial CRC value
  if (!direct) {
    crcinit_nondirect=crcinit;
    crc=crcinit;
    for (int i=0; i<order; i++) {
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
    for (int i=0; i<order; i++) {
      bit=crc & 1;
      if (bit) crc^= polynom;
      crc >>= 1;
      if (bit) crc|= crchighbit;
    }
    crcinit_nondirect=crc;
  }

  return(crcbitbybitfast(msgptr, msglen(msgptr)));
}