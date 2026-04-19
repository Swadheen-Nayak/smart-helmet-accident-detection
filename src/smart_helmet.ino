#include <Wire.h>

const int MPU = 0x68;
long AcX, AcY, AcZ, GyX, GyY, GyZ;
float accelMagnitude, gyroMagnitude;

float accelThreshold = 1.8; // Sudden impact (in g)
float gyroThreshold = 100;  // Sudden rotation (in °/s)

const int buzzerPin = 8; // Buzzer connected to digital pin 8

unsigned long lastReadTime = 0;
unsigned long readInterval = 100; // Read every 100 ms

unsigned long pauseUntil = 0; // When to resume updates
bool paused = false;

unsigned long lastBeepTime = 0;
bool buzzerState = false;
unsigned long beepDuration = 300;   // ms ON
unsigned long beepPause = 200;      // ms OFF
unsigned long beepEndTime = 0;
bool beepingActive = false;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0); 
  Wire.endTransmission(true);

  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  Serial.println("Smart Helmet System Active...");
}

void loop() {
  unsigned long currentTime = millis();

  // ----- Handle pause after accident -----
  if (paused) {
    if (currentTime < pauseUntil) {
      handleBeeping(currentTime);
      return; // Skip reading MPU while paused
    } else {
      paused = false;
      beepingActive = false;
      digitalWrite(buzzerPin, LOW);
      Serial.println("✅ Resuming sensor updates...");
    }
  }

  // ----- Read MPU at fixed intervals -----
  if (currentTime - lastReadTime >= readInterval) {
    lastReadTime = currentTime;
    readMPU();
    detectAccident();
  }

  handleBeeping(currentTime);
}

void readMPU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // skip temperature
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  if (AcX == 0 && AcY == 0 && AcZ == 0 && GyX == 0 && GyY == 0 && GyZ == 0) {
    Serial.println("⚠️ MPU6050 stuck, resetting...");
    resetMPU();
    return;
  }

  float Ax = AcX / 16384.0;
  float Ay = AcY / 16384.0;
  float Az = AcZ / 16384.0;
  float Gx = GyX / 131.0;
  float Gy = GyY / 131.0;
  float Gz = GyZ / 131.0;

  accelMagnitude = sqrt(Ax*Ax + Ay*Ay + Az*Az);
  gyroMagnitude = sqrt(Gx*Gx + Gy*Gy + Gz*Gz);

  Serial.print("Accel(g): ");
  Serial.print(accelMagnitude);
  Serial.print(" | Gyro(°/s): ");
  Serial.println(gyroMagnitude);
}

void detectAccident() {
  if ((accelMagnitude > accelThreshold || accelMagnitude < 0.5) && gyroMagnitude > gyroThreshold) {
    Serial.println("🚨 Accident Detected! (Strong impact + sudden rotation)");

    startBeeping(5000);    // Beep for 5 seconds
    pauseReadings(5000);   // Pause sensor updates for 5 seconds
  }
}

void startBeeping(unsigned long duration) {
  beepingActive = true;
  beepEndTime = millis() + duration;
  lastBeepTime = millis();
  buzzerState = true;
  digitalWrite(buzzerPin, HIGH);
}

void handleBeeping(unsigned long currentTime) {
  if (!beepingActive) return;

  if (currentTime >= beepEndTime) {
    beepingActive = false;
    digitalWrite(buzzerPin, LOW);
    return;
  }

  if (buzzerState && currentTime - lastBeepTime >= beepDuration) {
    buzzerState = false;
    digitalWrite(buzzerPin, LOW);
    lastBeepTime = currentTime;
  } 
  else if (!buzzerState && currentTime - lastBeepTime >= beepPause) {
    buzzerState = true;
    digitalWrite(buzzerPin, HIGH);
    lastBeepTime = currentTime;
  }
}

void pauseReadings(unsigned long duration) {
  paused = true;
  pauseUntil = millis() + duration;
  Serial.println("⏸️ Pausing sensor updates for safety...");
}

void resetMPU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x80);
  Wire.endTransmission(true);
  delay(100);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  Serial.println("✅ MPU6050 reinitialized.");
}
