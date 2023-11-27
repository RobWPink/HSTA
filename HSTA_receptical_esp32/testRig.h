#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Wire.h>
#include <Regexp.h>

#define I2C_ADR 0x03
#define I2C_SCL 1
#define I2C_SDA 2
#define I2C_FRQ 400000
#define POLL_TIMEOUT 5000

unsigned long long max_filesize = 10*pow(10,6);
unsigned long timer[10] = {0};

String logFile = "/HSTA.log";
String headerString = "time,pressure,temperature\n";
String date;
String psi;
String temp1;
String temp2;;
String temp3;

