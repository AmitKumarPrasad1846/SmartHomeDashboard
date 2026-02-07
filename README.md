# 🌅 ESP8266 Smart Home Dashboard

A beautiful, responsive web-based smart home control panel with sunset glass theme, built on ESP8266.

![Smart Home Dashboard](https://img.shields.io/badge/Platform-ESP8266-orange)
![License](https://img.shields.io/badge/License-MIT-green)
![Version](https://img.shields.io/badge/Version-1.0.0-blue)

## ✨ Features

- **🌡️ Temperature & Humidity Monitoring** - Real-time DHT11 sensor readings
- **🔆 Ambient Light Detection** - Automatic day/night detection with LDR
- **💡 Smart Device Control** - Control relays and LED via web interface
- **🎨 Beautiful UI** - Sunset glass theme with smooth animations
- **📱 Responsive Design** - Works on desktop, tablet, and mobile
- **🔌 Manual/Auto Modes** - Switch between automatic and manual control
- **⚡ Real-time Updates** - Auto-refreshes every 3 seconds

## 📋 Hardware Requirements

| Component | Quantity | Connection |
|-----------|----------|------------|
| ESP8266 (NodeMCU) | 1 | Main Controller |
| DHT11 Sensor | 1 | D1 (GPIO5) |
| LDR with 10K resistor | 1 | D2 (GPIO4) |
| 5V Relay Module | 2 | D5 (GPIO14), D6 (GPIO12) |
| LED (with resistor) | 1 | D3 (GPIO0) |
| Breadboard & Jumper Wires | - | - |
| 5V Power Supply | 1 | - |

## 🔧 Pin Configuration

| Pin | GPIO | Component | Function |
|-----|------|-----------|----------|
| D1 | GPIO5 | DHT11 | Temperature/Humidity |
| D2 | GPIO4 | LDR | Light Detection |
| D3 | GPIO0 | LED | Status Indicator |
| D5 | GPIO14 | Relay 1 | Living Room Light |
| D6 | GPIO12 | Relay 2 | Bedroom Fan |

## 🚀 Installation

### 1. Hardware Setup
1. Connect all components as per the pin configuration
2. For LDR: Connect one end to 3.3V, other end to D2 with 10K pull-down resistor
3. For DHT11: Connect VCC to 3.3V, GND to GND, DATA to D1
4. For Relays: Connect IN pins to D5/D6, VCC to 5V, GND to GND

### 2. Software Setup
1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Add ESP8266 board support:
   - File → Preferences → Additional Boards Manager URLs:
   - Add: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
3. Install required libraries:
   - ESP8266WiFi
   - ESP8266WebServer
   - DHT sensor library (by Adafruit)

### 3. Configuration
1. Open `SmartHomeDashboard.ino` in Arduino IDE
2. Update WiFi credentials:
   ```cpp
   #define WIFI_SSID "Your_WiFi_SSID"
   #define WIFI_PASSWORD "Your_WiFi_Password"