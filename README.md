# 🔦 BLE-Enabled Light Tracking System

A servo-based light-tracking system powered by an **ESP32** microcontroller and controlled using **Bluetooth Low Energy (BLE)**. This project uses **Light Dependent Resistors (LDRs)** to detect the direction of the strongest light source and aligns two servos for accurate tracking.

## 📌 Overview

This system uses:
- Two **servos** for horizontal (pan) and vertical (tilt) movement.
- Two **LDR sensors** to detect light intensity from different directions.
- An **ESP32** microcontroller to process sensor data and control servos.
- **Bluetooth Low Energy (BLE)** for real-time communication with external devices (e.g., smartphones or laptops).

It continuously reads the LDR sensor values, calculates the optimal orientation, and positions the servos to align with the light source. The system also transmits real-time data (e.g., current, max, and min light levels) via BLE.

## ⚙️ Features

- 📡 **Real-time BLE communication** with smartphones/laptops
- 🔄 **Dual-axis servo control** for pan and tilt
- 🔦 **LDR-based light sensing**
- 🧠 **Edge-case handling** for complete darkness
- 🖥️ **Serial debugging** for data monitoring and calibration
- 📈 **Mapped and scaled light intensity values** (0–100) for precision

## 🛠️ Technologies Used

- **ESP32 Dev Board**
- **Arduino IDE**
- **C++ / Arduino**
- **LDR sensors**
- **Servo motors**
- **BLE libraries**
- **Serial Monitor (for debugging)**

## 🚀 Applications

This light-following system can be applied in:

- ☀️ **Solar tracking systems**
- 🤖 **Robotic vision platforms**
- 🎯 **Sensor-actuator integration projects**
- 🔬 **Interactive art installations**

## 📸 Demo

<!-- Optionally embed a video or image of the project in action -->
![servo-tracking-demo](demo.gif)


