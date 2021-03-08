
/**
   @file example.ino
   @author Felix Galindo
   @date June 2017
   @brief Example using HPMA115S0 sensor library on a Feather 32u4
   @license MIT
*/

#include <Arduino.h>
#include <hpma115S0.h>
#include <SoftwareSerial.h>

//Create an instance of software serial
SoftwareSerial hpmaSerial(A0, 2); // Feather TX, Feather RX

//Create an instance of the hpma115S0 library
HPMA115S0 hpma115S0(hpmaSerial);

void setup() {
  Serial.begin(9600);
  hpmaSerial.begin(9600);
  delay(2000);
  Serial.println("Starting...");
  hpma115S0.SetAdjustmentCoefficient(120); // 30-200
  hpma115S0.Init();
  delay(100);
  hpma115S0.StartParticleMeasurement();
}

void loop() {
  unsigned int pm2_5, pm10;
  if (hpma115S0.ReadParticleMeasurement(&pm2_5, &pm10)) {
    Serial.print("PM 2.5:\t" + String(pm2_5) + " ug/m3\t" );
    Serial.println("\tPM 10:\t" + String(pm10) + " ug/m3" );
  }
  delay(2000);
}
