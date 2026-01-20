#include <NewPing.h> 
#include <BluetoothSerial.h> // Add Bluetooth Serial 
BluetoothSerial SerialBT; // Create a Bluetooth Serial object 
#define TRIG_PIN 5 
#define ECHO_PIN 18 
NewPing sonar(TRIG_PIN, ECHO_PIN, 50); // Steering hand & fall detection 
const int blinkSensorPin = 25; 
const int blinkThreshold = 500; 
const int eyeClosedTimeLimit = 2000; // 2 seconds 
const int handThreshold = 20; 
const int faceCloseThreshold = 5; 
const int handsOffWarningTime = 3000; 
unsigned long eyeCloseStartTime = 0; 
bool eyesClosed = false; 
bool handsDetected = true; 
unsigned long handsOffStart = 0; 
const int buzzerPin = 4; 
void setup() { 
Serial.begin(115200); 
SerialBT.begin("ESP32_Drowsiness"); // Bluetooth device name 
 Serial.println("Bluetooth started. Connect to 'ESP32_Drowsiness'"); 
  SerialBT.println("Bluetooth Ready!"); 
 
  pinMode(blinkSensorPin, INPUT); 
  pinMode(buzzerPin, OUTPUT); 
  digitalWrite(buzzerPin, LOW); 
} 
 
void loop() { 
  int sensorValue = analogRead(blinkSensorPin); 
  int handDistance = sonar.ping_cm(); 
 
  // EYE BLINK MONITORING 
  if (sensorValue > blinkThreshold) { 
    if (eyeCloseStartTime == 0) { 
      eyeCloseStartTime = millis(); 
    } 
    if (millis() - eyeCloseStartTime >= eyeClosedTimeLimit) { 
      if (!eyesClosed) { 
        String msg = "DROWSY! Stay Alert! Buzzer ON"; 
        Serial.println(msg); 
        SerialBT.println(msg); 
        digitalWrite(buzzerPin, HIGH); 
        eyesClosed = true; 
      } 
    } 
  } else { 
    if (eyesClosed) {
       String msg = "Eyes Open - Buzzer OFF"; 
      Serial.println(msg); 
      SerialBT.println(msg); 
      digitalWrite(buzzerPin, LOW);
      eyesClosed = false; 
    } 
    eyeCloseStartTime = 0; 
  } 
 
  // STEERING WHEEL MONITORING  
  if (handDistance > 0 && handDistance < handThreshold) { 
    if (!handsDetected) { 
      String msg = "Hands on Wheel - Normal"; 
      Serial.println(msg); 
      SerialBT.println(msg); 
      handsDetected = true; 
      handsOffStart = 0; 
    } 
  } else { 
    if (handsDetected) { 
      String msg = "Hands Off Detected - Monitoring..."; 
      Serial.println(msg); 
      SerialBT.println(msg); 
      handsDetected = false; 
      handsOffStart = millis(); 
    } 
  } 
 
  if (!handsDetected && handsOffStart > 0 && millis() - handsOffStart > handsOffWarningTime)  
{ 
    String msg = "Hold the Wheel!"; 
    Serial.println(msg); 
    SerialBT.println(msg); 
    digitalWrite(buzzerPin, HIGH); 
    delay(1000); 
    digitalWrite(buzzerPin, LOW);
      } 
 
  // FALLING ONTO STEERING DETECTION  
  if (handDistance > 0 && handDistance < faceCloseThreshold) { 
    String msg = "EMERGENCY! Driver Unconscious? Buzzer ON"; 
    Serial.println(msg); 
    SerialBT.println(msg); 
    digitalWrite(buzzerPin, HIGH); 
    delay(3000); 
    digitalWrite(buzzerPin, LOW); 
  } 
 
  delay(100); 
} 
 