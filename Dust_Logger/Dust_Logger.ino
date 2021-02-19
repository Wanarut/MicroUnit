#include <RTClib.h>
#include <SD.h>

RTC_DS1307 rtc;
const int chipSelect = 10;
File logFile;
String logFileName;
bool hasRTC = false;
bool hasSD = false;
bool deleteDupicateFile = true;
//bool deleteDupicateFile = false;

const int sharpSensor_1[] = {2, A0};
const int sharpSensor_2[] = {3, A1};

// For averaging last N raw voltage readings.
#define N 100
// Set the typical output voltage in Volts when there is zero dust.
static float Voc[] = {0.6, 0.6};
// Use the typical sensitivity in units of V per 100ug/m3.
const float K = 0.5;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);
  //Check Module
  checkModule();
  if (hasSD) createlogFile();

  pinMode(sharpSensor_1[0], OUTPUT);
  pinMode(sharpSensor_2[0], OUTPUT);
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
    String Data_1 = String(1.0 * getDustDensity(sharpSensor_1[0], sharpSensor_1[1], 0));
    String Data_2 = String(1.5 * getDustDensity(sharpSensor_2[0], sharpSensor_2[1], 1));
    String record = date_now + ',' + time_now + ',' + Data_1 + ',' + Data_2;
    String record_2 = date_now + ' ' + time_now + '\t' + Data_1 + '\t' + Data_2;

    if (hasSD) {
      logFile = SD.open(logFileName.c_str(), FILE_WRITE);
      logFile.println(record);
      logFile.close();
    }
    Serial.println(record_2);

    loop_pre_mil = loop_cur_mil;
  }
}

static unsigned long cur_mil;
static unsigned long pre_mil;
float getDustDensity(int sharpLEDPin, int sharpVoPin, int i) {
  unsigned long VoRawTotal = 0;
  int VoRawCount = 0;
  while (true) {
    cur_mil = millis();
    // Wait for remainder of the 10ms cycle
    if (cur_mil - pre_mil >= 10) {
      // Turn on the dust sensor LED by setting digital pin LOW.
      digitalWrite(sharpLEDPin, LOW);
      // Wait 0.28ms before taking a reading of the output voltage as per spec.
      delayMicroseconds(280);
      // Record the output voltage. This operation takes around 100 microseconds.
      int VoRaw = analogRead(sharpVoPin);
      // Turn the dust sensor LED off by setting digital pin HIGH.
      digitalWrite(sharpLEDPin, HIGH);

      VoRawTotal += VoRaw;
      VoRawCount++;
      pre_mil = cur_mil;

      if ( VoRawCount >= N ) {
        return calculateDustDensity(1.0 * VoRawTotal / N, i);
      }
    }
  }
}

float calculateDustDensity(float aVo, int i) {
  // Compute the output voltage in Volts.
  float Vo = aVo / 1024.0 * 5.0;

  // Convert to Dust Density in units of ug/m3.
  float dV = Vo - Voc[i];
  if ( dV < 0 ) {
    dV = 0;
    Voc[i] = Vo;
  }
  float dustDensity = dV / K * 100.0;
  return dustDensity;
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
      logFile.println("Date,Time,Data_1,Data_2");
      logFile.close();
    }
  } else {
    Serial.println(logFileName + " doesn't exist.");

    Serial.println("Creating " + logFileName);
    logFile = SD.open(logFileName.c_str(), FILE_WRITE);
    logFile.println("Date,Time,Data_1,Data_2");
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
