#include <RunningAverage.h>
#include <LedHelper.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h> //For I2C comms
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_LiquidCrystal.h"
#include <bb_rtc.h>

BBRTC rtc;
SmallMatrix smallMatrix[3] = {SmallMatrix(0x70), SmallMatrix(0x71), SmallMatrix(0x72) };
LargeMatrix bigMatrix[3] = {LargeMatrix(0x73), LargeMatrix(0x74), LargeMatrix(0x75) };
Adafruit_LiquidCrystal lcd(0);
Adafruit_MCP9600 mcp;

#define ESTOP_BREAK 40
#define LED_PWR 22
#define TRACO_24VDC 23

#define SS 10    //W5500 CS
#define RST 7    //W5500 RST
#define CS 4     //SD CS pin

#define PT -1
#define TT -1

#define BUTTON -1
#define VALVE -1

uint16_t mcpAddr = 0x67;

volatile int  MOVING_AVG_SIZE = 10;

unsigned long daughterTimer = 0;
unsigned long debouncer = 0;
bool tog = false;