#include <RTClib.h>
#include <SD.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <PMS.h>

RTC_DS1307 rtc;
const int chipSelect = 10;
File logFile;
String logFileName;
bool hasRTC = false;
bool hasSD = false;
//bool deleteDupicateFile = true;
bool deleteDupicateFile = false;

SoftwareSerial pmsSerial_0(A0, 2); // Feather TX, Feather RX
SoftwareSerial pmsSerial_1(A1, 3); // Feather TX, Feather RX
PMS pms_0(pmsSerial_0);
PMS pms_1(pmsSerial_1);
PMS::DATA data_0;
PMS::DATA data_1;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pmsSerial_0.begin(9600);
  pmsSerial_1.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);
  while (!pmsSerial_0);
  while (!pmsSerial_1);

  //Check Module
  checkModule();
  if (hasSD) createlogFile();

  // Wait two seconds for startup.
  delay(2000);
  Serial.println("===================");
  Serial.println("!Start Dust Looger!");
  Serial.println("===================");
}

static unsigned long loop_cur_mil;
static unsigned long loop_pre_mil;
void loop() {
  loop_cur_mil = millis();
  if (loop_cur_mil - loop_pre_mil >= 1000) {
    String date_now = getDateString();
    String time_now = getTimeString();

    unsigned int pm01_0, pm25_0, pm10_0, pm01_1, pm25_1, pm10_1;
    pmsSerial_0.listen();
    while (!pms_0.read(data_0));
    pmsSerial_1.listen();
    while (!pms_1.read(data_1));

    pm01_0 = data_0.PM_AE_UG_1_0;
    pm25_0 = data_0.PM_AE_UG_2_5;
    pm10_0 = data_0.PM_AE_UG_10_0;

    pm01_1 = data_1.PM_AE_UG_1_0;
    pm25_1 = data_1.PM_AE_UG_2_5;
    pm10_1 = data_1.PM_AE_UG_10_0;
    
    float pm25_in, pm25_out;
    if (pm25_0 > pm25_1) {
      pm25_in = pm25_0;
      pm25_out = pm25_1;
    } else {
      pm25_in = pm25_1;
      pm25_out = pm25_0;
    }
    float Eff = (pm25_in - pm25_out) / (pm25_in + 1);

    String Data_1 = String(pm25_in);
    String Data_2 = String(pm25_out);
    String Data_3 = String(Eff);
    String record = date_now + ',' + time_now + ',' + Data_1 + ',' + Data_2 + ',' + Data_3;
    String record_2 = date_now + ' ' + time_now + '\t' + Data_1 + '\t' + Data_2 + '\t' + Data_3;

    if (hasSD) {
      logFile = SD.open(logFileName.c_str(), FILE_WRITE);
      logFile.println(record);
      logFile.close();
    }
    Serial.println(record_2);

    loop_pre_mil = loop_cur_mil;
  }
}

void checkModule() {
  if (! rtc.begin()) {
    Serial.println("RTC Failed!");
  } else {
    Serial.println("Found RTC.");
    hasRTC = true;
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card Failed!");
  } else {
    Serial.println("Found SD Card.");
    hasSD = true;
  }
}

void createlogFile() {
  String date_now = getDateString('_');
  logFileName = date_now + ".csv";
  logFileName = logFileName.substring(2);

  if (SD.exists(logFileName.c_str())) {
    Serial.println(logFileName + " exists.");
    if (deleteDupicateFile) {
      SD.remove(logFileName.c_str());
      Serial.println("Removing " + logFileName);
      Serial.println("Creating " + logFileName);
      logFile = SD.open(logFileName.c_str(), FILE_WRITE);
      logFile.println("Date,Time,PM2.5_in,PM2.5_out,Efficient");
      logFile.close();
    }
  } else {
    Serial.println(logFileName + " doesn't exist.");

    Serial.println("Creating " + logFileName);
    logFile = SD.open(logFileName.c_str(), FILE_WRITE);
    logFile.println("Date,Time,PM2.5_in,PM2.5_out,Efficient");
    logFile.close();
  }
}

String getDateString() {
  return getDateString('/');
}

String getDateString(char sym) {
  if (hasRTC) {
    DateTime now = rtc.now();
    return String(now.year()) + sym + String(now.month()) + sym + String(now.day());
  }
  return "1970/1/1";
}

String getTimeString() {
  if (hasRTC) {
    DateTime now = rtc.now();
    return String(now.hour()) + ':' + String(now.minute()) + ':' + String(now.second());
  }
  return String(millis());
}
