#include <RunningAverage.h>
#include <LedHelper.h>
#include <Adafruit_GFX.h>
#include <Wire.h> //For I2C comms
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_LiquidCrystal.h"
#include <bb_rtc.h>
#define TCAADDR 0x70
BBRTC rtc;
SmallMatrix smallMatrix[3] = {SmallMatrix(0x70), SmallMatrix(0x71), SmallMatrix(0x72) };
LargeMatrix bigMatrix[3] = {LargeMatrix(0x73), LargeMatrix(0x74), LargeMatrix(0x75) };
Adafruit_LiquidCrystal lcd(0);

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

#define PT A11
#define TT A5

#define SWITCH 47
#define VENT 45
#define VALVE 43

/*
#define RLY0 47
#define RLY1 45
#define RLY2 43
#define RLY3 41
#define RLY4 49
#define RLY5 48
#define RLY6
#define RLY7
#define RLY8
*/

char buffer[32] = {"\0"};
volatile int  MOVING_AVG_SIZE = 10;
double currentPressure = 0;
double currentTemperature = 0;
unsigned long daughterTimer = 0;
bool tog = false;
unsigned long timer[3] = {0};
