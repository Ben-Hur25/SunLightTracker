#include <ESP32Servo.h>  
#include <BLEDevice.h>   
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// Define UUIDs for the BLE service and characteristic
#define SERVICE_UUID "2febd746-3cf0-4dad-9b4d-62f93d8aa6ec"         // Unique identifier for the BLE service
#define CHARACTERISTIC_UUID "037da031-1505-4ae8-a4ea-bf8c36bae3e9"  // Unique identifier for the BLE characteristic

// Servo objects for horizontal and tilt movement
Servo servoHorizontal;
Servo servoTilt;

// GPIO pins connected to LDR sensors
int ldrHorizontalPin = 16;  // Analog pin for the horizontal LDR
int ldrTiltPin = 17;        // Analog pin for the tilt LDR

// GPIO pins connected to servos
int servoHorizontalPin = 13;  // GPIO pin for horizontal servo
int servoTiltPin = 14;        // GPIO pin for tilt servo

// Current positions of the servos
int horizontalPosition = 90;  // Initial position for the horizontal servo
int tiltPosition = 90;        // Initial position for the tilt servo

// Variables for tracking the highest and lowest LDR values (scaled to 0–100)
int highestHorizontalValue = 0;   // Maximum horizontal LDR value recorded
int lowestHorizontalValue = 100;  // Minimum horizontal LDR value recorded
int highestTiltValue = 0;         // Maximum tilt LDR value recorded
int lowestTiltValue = 100;        // Minimum tilt LDR value recorded

// Step size for smooth servo movement
const int stepSize = 1;  // Smaller values result in smoother but slower movements

// Threshold values for stopping servo movement
const int horizontalThreshold = 70;  // Threshold for the horizontal LDR
const int tiltThreshold = 70;        // Threshold for the tilt LDR

// Pointer to the BLE characteristic for sending data to the client
BLECharacteristic *pCharacteristic;

// BLE server callback class to handle connection events
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    Serial.println("Device connected");  // Log when a device connects
  }

  void onDisconnect(BLEServer *pServer) {
    Serial.println("Device disconnected");  // Log when a device disconnects
    BLEDevice::startAdvertising();          // Restart advertising to allow new connections
  }
};

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Configure LDR pins as input
  pinMode(ldrHorizontalPin, INPUT);
  pinMode(ldrTiltPin, INPUT);

  // Attach servos to their respective GPIO pins
  servoHorizontal.attach(servoHorizontalPin);
  servoTilt.attach(servoTiltPin);

  // Set initial servo positions
  servoHorizontal.write(horizontalPosition);
  servoTilt.write(tiltPosition);

  // Initialize BLE and set the device name
  BLEDevice::init("SunLightTracker");

  // Create BLE server and set callback functions for connection events
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE characteristic with read and notify properties
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  // Add a descriptor to enable notifications on the characteristic
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the BLE service and advertising
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  Serial.println("BLE advertising started");  // Log that BLE advertising has started
}

void loop() {
  // Read and scale LDR values from raw ADC readings (0–4095 to 0–100, inverted)
  int ldrHorizontalValue = map(analogRead(ldrHorizontalPin), 0, 4095, 100, 0);
  int ldrTiltValue = map(analogRead(ldrTiltPin), 0, 4095, 100, 0);

  // Update highest and lowest recorded values for horizontal LDR
  if (ldrHorizontalValue > highestHorizontalValue) {
    highestHorizontalValue = ldrHorizontalValue;
  }
  if (ldrHorizontalValue < lowestHorizontalValue) {
    lowestHorizontalValue = ldrHorizontalValue;
  }

  // Update highest and lowest recorded values for tilt LDR
  if (ldrTiltValue > highestTiltValue) {
    highestTiltValue = ldrTiltValue;
  }
  if (ldrTiltValue < lowestTiltValue) {
    lowestTiltValue = ldrTiltValue;
  }

  // Print LDR values and recorded extremes to the serial monitor for debugging
  Serial.print("Horizontal LDR: ");
  Serial.print(ldrHorizontalValue);
  Serial.print(" (High: ");
  Serial.print(highestHorizontalValue);
  Serial.print(", Low: ");
  Serial.print(lowestHorizontalValue);
  Serial.print(") | Tilt LDR: ");
  Serial.print(ldrTiltValue);
  Serial.print(" (High: ");
  Serial.print(highestTiltValue);
  Serial.print(", Low: ");
  Serial.print(lowestTiltValue);
  Serial.println(")");

  // Check if horizontal LDR value is below the threshold; update servo if true
  if (ldrHorizontalValue <= horizontalThreshold) {
    // Calculate target position for horizontal servo
    int targetHorizontalPosition = (ldrHorizontalValue > 50) ? 180 : 0;
    if (horizontalPosition < targetHorizontalPosition) {
      horizontalPosition += stepSize;
    } else if (horizontalPosition > targetHorizontalPosition) {
      horizontalPosition -= stepSize;
    }
    servoHorizontal.write(horizontalPosition);  // Move horizontal servo
  }

  // Check if tilt LDR value is below the threshold; update servo if true
  if (ldrTiltValue <= tiltThreshold) {
    // Calculate target position for tilt servo
    int targetTiltPosition = (ldrTiltValue > 50) ? 180 : 90;
    if (tiltPosition < targetTiltPosition) {
      tiltPosition += stepSize;
    } else if (tiltPosition > targetTiltPosition) {
      tiltPosition -= stepSize;
    }
    servoTilt.write(tiltPosition);  // Move tilt servo
  }

  // Combine LDR values and recorded extremes into a single string for BLE notification
  String data = String("Horizontal: ") + String(ldrHorizontalValue) + String(" (High: ") + String(highestHorizontalValue) + String(", Low: ") + String(lowestHorizontalValue) + String(") | Tilt: ") + String(ldrTiltValue) + String(" (High: ") + String(highestTiltValue) + String(", Low: ") + String(lowestTiltValue) + ")";

  // Send the LDR data to the BLE client
  pCharacteristic->setValue(data.c_str());
  pCharacteristic->notify();

  // Wait before the next iteration for smooth operation
  delay(20);  // Short delay for smoother servo movement
}