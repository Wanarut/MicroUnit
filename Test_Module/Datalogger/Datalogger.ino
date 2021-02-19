/*
  SD card basic file example
  This example shows how to create and destroy an SD card file
  The circuit. Pin numbers reflect the default
  SPI pins for Uno and Nano models:
   SD card attached to SPI bus as follows:
 ** SDO - pin 11
 ** SDI - pin 12
 ** CLK - pin 13
 ** CS - depends on your SD card shield or module.
        Pin 10 used here for consistency with other Arduino examples
    (for MKRZero SD: SDCARD_SS_PIN)
  created   Nov 2010
  by David A. Mellis
  modified 24 July 2020
  by Tom Igoe
  This example code is in the public domain.

*/
#include <SD.h>
const int chipSelect = 10;
File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);
  Serial.print("Initializing SD card...");
  
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  
  if (SD.exists("21-2-18.csv")) {
    Serial.println("21-2-18.csv exists.");
  } else {
    Serial.println("21-2-18.csv doesn't exist.");
  }

  // open a new file and immediately close it:
  Serial.println("Creating 21-2-18.csv...");
  myFile = SD.open("21-2-18.csv", FILE_WRITE);
  myFile.println("date,time,data_1,data_2");
  myFile.println("2021/2/18,12:31:52,154,23");
  myFile.close();

  // Check to see if the file exists:
  if (SD.exists("21-2-18.csv")) {
    Serial.println("21-2-18.csv exists.");
  } else {
    Serial.println("21-2-18.csv doesn't exist.");
  }

  // delete the file:
  Serial.println("Removing 21-2-18.csv...");
  SD.remove("21-2-18.csv");
  if (SD.exists("21-2-18.csv")) {
    Serial.println("2021_2_18.csv exists.");
  } else {
    Serial.println("2021_2_18.csv doesn't exist.");
  }
}

void loop() {
  // nothing happens after setup finishes.
}
