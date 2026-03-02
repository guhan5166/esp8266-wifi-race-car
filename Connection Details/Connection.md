# 🔌 Connection Details

## 🧠 Overview

The system uses an **ESP8266 (NodeMCU)** to control two DC motors via an **L298N motor driver** and a **buzzer** for horn functionality.
All components share a **common ground**.

---

## ⚙️ NodeMCU → L298N Motor Driver

| NodeMCU Pin | GPIO   | Connect To (L298N) | Purpose                 |
| ----------- | ------ | ------------------ | ----------------------- |
| D1          | GPIO5  | ENA                | Right Motor Speed (PWM) |
| D5          | GPIO14 | IN1                | Right Motor Direction   |
| D6          | GPIO12 | IN2                | Right Motor Direction   |
| D7          | GPIO13 | IN3                | Left Motor Direction    |
| D8          | GPIO15 | IN4                | Left Motor Direction    |
| D2          | GPIO4  | ENB                | Left Motor Speed (PWM)  |

---

## 🔊 Buzzer Connection

| Buzzer Pin   | NodeMCU Pin | GPIO  |
| ------------ | ----------- | ----- |
| + (Positive) | D3          | GPIO0 |
| – (Negative) | GND         | GND   |

**Note:**
GPIO0 (D3) must stay LOW during boot.
Make sure the buzzer does not pull it HIGH when powering on.

---

## 🔋 Motor Connections (L298N)

| L298N Output | Connect To     |
| ------------ | -------------- |
| OUT1 & OUT2  | Right DC Motor |
| OUT3 & OUT4  | Left DC Motor  |

---

## 🔌 Power Connections

### Motor Driver Power

* 12V → Battery Positive
* GND → Battery Negative

### NodeMCU Power

* Power via USB
  OR
* Use regulated 5V to VIN pin

### ⚠️ Important

* **Common Ground Required**
  Connect NodeMCU GND to L298N GND.
* Do NOT power motors directly from NodeMCU.

---

## 📡 Network Setup

1. Update WiFi credentials in code:

   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_PASSWORD";
   ```
2. Upload code.
3. Open Serial Monitor (115200 baud).
4. Note the IP address.
5. Open that IP in your mobile browser.

---

## 🧩 System Flow

Mobile Browser
↓
WebSocket (Port 81)
↓
ESP8266
↓
L298N Motor Driver
↓
DC Motors

Separate channel:
Buzzer Button → GPIO0 → Buzzer

---
