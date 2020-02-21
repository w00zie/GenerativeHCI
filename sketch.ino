#include <Wire.h> // Used to establied serial communication on the I2C bus
#include "SparkFunTMP102.h" // Used to send and recieve specific information from temperature sensor
#define lightSensor A3
#define micSensor A1

TMP102 sensor0(0x48); // Initialize sensor at I2C address 0x48
float amplitude;
double volts;
unsigned long duration, start;
int lightIntensity, temperature, distance;
unsigned int peakToPeak, signalMin, signalMax;
const unsigned int trigPin = 3;
const unsigned int echoPin = 2;
bool knock;
const int sampleWindow = 250; // Microphone sample window width in mS (250 mS = 4Hz)

void setup() {
  Wire.begin(); //Join I2C Address
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600); // Start serial communication at 9600 baud
  sensor0.begin(); // Join I2C bus
  sensor0.setConversionRate(2); // 4Hz
  sensor0.setExtendedMode(0); //0:12-bit Temperature (-55C to +128C)
}

void loop() {
  // Temperature
  sensor0.wakeup();
  temperature = sensor0.readTempC();
  sensor0.sleep();
  
  // Light
  lightIntensity = analogRead(lightSensor);
  
  // Ultrasounds
  digitalWrite(trigPin, LOW);
  delayMicroseconds(1);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  distance = duration/58.2; // In cm
  if (distance >= 200 || distance <= 0) {
    distance = -1;
  }

  // Microphone
  peakToPeak = 0;
  signalMax = 0;
  signalMin = 1024;
  knock = false;
  start = millis();
  while (millis() - start < sampleWindow) {
    amplitude = analogRead(micSensor);
    if (amplitude < 1024) {  //This is the max of the 10-bit ADC so this loop will include all readings
      if (amplitude > signalMax) {
        signalMax = amplitude;  // save just the max levels
      }
      else if (amplitude < signalMin) {
        signalMin = knock;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;
  volts = (peakToPeak*5)/1024;
  if (volts >= 3.5) {
    knock = true;
  }
  
  // Prints
  Serial.print(temperature);
  Serial.print(" ");
  Serial.print(lightIntensity);
  Serial.print(" ");
  Serial.print(distance);
  Serial.print(" ");
  Serial.println(knock);
}
