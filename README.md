# 🚨 Smart Helmet Accident Detection System

## 📌 Overview
Arduino-based system that detects accidents using MPU6050 (accelerometer + gyroscope) and triggers a buzzer alert in real time.

---

## ⚙️ Features
- Real-time motion monitoring
- Accident detection using impact + rotation
- Buzzer alert system
- Temporary pause after detection for stability
- MPU6050 auto-reset on sensor failure

---

## 🧠 Working Principle
The system continuously reads acceleration and angular velocity.  
If sudden impact + rotation crosses threshold → accident is detected → buzzer activates.

---

## 🔌 Hardware Used
- Arduino UNO
- MPU6050 Sensor
- Buzzer
- Jumper wires

---

## 📂 Code
Main logic is inside:
`src/smart_helmet.ino`

---

## 🚀 Future Improvements
- GPS tracking for location alert
- GSM module for emergency SMS
- Mobile app integration

---

## 👨‍💻 Status
Prototype stage (working model)
