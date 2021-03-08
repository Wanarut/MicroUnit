/////////////////////////////////////////////////////////////////////////////
// Sharp GP2Y1014AU0F Dust Sensor Demo
//
// Board Connection:
//   GP2Y1014    Arduino
//   V-LED       Between R1 and C1
//   LED-GND     C1 and GND
//   LED         Pin 7
//   S-GND       GND
//   Vo          A5
//   Vcc         5V
//
// Serial monitor setting:
//   9600 baud
/////////////////////////////////////////////////////////////////////////////

// Arduino pin numbers.
const int sharpLEDPin = 2;   // Arduino digital pin 7 connect to sensor LED.
const int sharpVoPin = A0;   // Arduino analog pin 5 connect to sensor Vo.

// For averaging last N raw voltage readings.
#define N 100

// Set the typical output voltage in Volts when there is zero dust.
static float Voc = 0.6;

// Use the typical sensitivity in units of V per 100ug/m3.
const float K = 0.11;

/////////////////////////////////////////////////////////////////////////////

// Arduino setup function.
void setup() {
  // Set LED pin for output.
  pinMode(sharpLEDPin, OUTPUT);

  // Start the hardware serial port for the serial monitor.
  Serial.begin(9600);

  // Wait two seconds for startup.
  delay(2000);
  Serial.println("");
  Serial.println("GP2Y1014AU0F Demo");
  Serial.println("=================");
}

// Arduino main loop.
void loop() {

  float dustDensity = getDustDensity(sharpLEDPin, sharpVoPin);
  Serial.println("DustDensity " + String(dustDensity) + " ug/m3");
  delay(1000);

}

static unsigned long cur_mil;
static unsigned long pre_mil;
float getDustDensity(int sharpLEDPin, int sharpVoPin) {
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
        return calculateDustDensity(1.0 * VoRawTotal / N);
      }
    }
  }
}

float calculateDustDensity(float aVo) {
  // Compute the output voltage in Volts.
  float Vo = aVo / 1024.0 * 5.0;

  // Convert to Dust Density in units of ug/m3.
  float dV = Vo - Voc;
  if ( dV < 0 ) {
    dV = 0;
    Voc = Vo;
  }
  float dustDensity = dV / K * 100.0;
  return dustDensity;
}
