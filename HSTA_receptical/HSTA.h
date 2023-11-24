#include <RunningAverage.h>
#include <LedHelper.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"
#include <Wire.h> //For I2C comms
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_LiquidCrystal.h"
#include <bb_rtc.h>

BBRTC rtc;
SmallMatrix smallMatrix[3] = {SmallMatrix(0x70), SmallMatrix(0x71), SmallMatrix(0x72) };
LargeMatrix bigMatrix[3] = {LargeMatrix(0x73), LargeMatrix(0x74), LargeMatrix(0x75) };
Adafruit_LiquidCrystal lcd(0);
Adafruit_MCP9600 mcp;

// SAE J2799 msg control codes
#define XBOF_sym 0xFF // Extra Begin of Frame
#define BOF_sym 0xC0 // Begin of Frame
#define EOF_sym 0xC1 // End of Frame
#define CE_sym 0x7D // Control Escape

#define ESTOP_BREAK 40
#define LED_PWR 22
#define TRACO_24VDC 23
#define REDE 12

#define SS 10    //W5500 CS
#define RST 7    //W5500 RST
#define CS 4     //SD CS pin

#define PT -1
#define TT -1

#define SWITCH -1
#define VALVE -1

uint16_t mcpAddr = 0x67;
char buffer[32] = {"\0"};
volatile int  MOVING_AVG_SIZE = 10;
double currentPressure = 0;
double currentTemperature = 0;
unsigned long daughterTimer = 0;
bool tog = false;
unsigned long timer[3] = {0};

//CRC constants:

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
