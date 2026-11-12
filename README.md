# Cloud-Connected Energy Meter Using ESP32 and INA219

## Project Overview

This project presents a cloud-connected energy monitoring system developed using an ESP32 microcontroller and an INA219 current and voltage sensor. The system measures electrical parameters, calculates power consumption and cumulative energy, and transmits the readings to an online dashboard using Wi-Fi and Adafruit IO.

The system enables real-time monitoring of electrical parameters and remote access to energy consumption data.

## Features

- Real-time voltage measurement.
- Current measurement using the INA219 sensor.
- Power calculation using voltage and current readings.
- Cumulative energy calculation in milliwatt-hours (mWh).
- Wireless data transmission using ESP32 Wi-Fi.
- Cloud-based monitoring using Adafruit IO.
- Persistent energy storage using ESP32 Preferences.
- Remote energy counter reset through an Adafruit IO feed.

## Hardware Components

| Component | Purpose |
|---|---|
| ESP32 Development Board | Main microcontroller and Wi-Fi communication |
| INA219 Sensor | Voltage and current measurement |
| 100 Ω Resistor | Electrical test load |
| DC Power Supply | Supplies power to the test circuit |
| Connecting Wires | Hardware connections |

## Software and Technologies

- Arduino IDE
- Embedded C/C++
- ESP32
- Adafruit INA219 Library
- Adafruit IO
- MQTT Protocol
- Preferences Library
- I²C Communication

## System Architecture

1. The INA219 sensor measures the voltage and current supplied to the load.
2. The ESP32 communicates with the INA219 through the I²C interface.
3. Electrical power is calculated using the measured voltage and current.
4. Energy consumption is calculated by integrating power over time.
5. The ESP32 transmits the measured parameters to Adafruit IO through Wi-Fi.
6. Adafruit IO displays the readings through an online dashboard.
7. The Preferences library stores cumulative energy data so it can be restored after a restart.

## Energy Calculation

Electrical power is calculated using:

**P = V × I**

Energy consumption is calculated by integrating power over time:

**E (mWh) = P (mW) × Δt (seconds) / 3600**

The accumulated energy value is stored in the ESP32's non-volatile Preferences storage.

## Adafruit IO Dashboard

The dashboard provides remote visualization of the measured electrical parameters.

**Live Dashboard:** [Adafruit IO Dashboard Link](https://io.adafruit.com/mihit_manoj/dashboards/virtual-energy-monitor?kiosk=true)

The dashboard includes the following feeds:

- Voltage
- Current
- Power
- Energy
- Energy Reset

## Hardware Implementation

The system was tested using a 100 Ω resistive load. Measurements were observed under both load ON and load OFF conditions to validate the monitoring functionality.

### Hardware Setup

![Hardware Setup](Images/hardware_setup.jpg)

### Dashboard – Load ON

![Load ON](Images/dashboard_load_on.png)

### Dashboard – Load OFF

![Load OFF](Images/dashboard_load_off.png)

## Repository Structure

```text
ESP32-Cloud-Connected-Energy-Meter/
│
├── README.md
├── Code/
│   └── energy_meter.ino
├── Documentation/
│   ├── Project_Report.pdf
│   └── Circuit_Diagram.png
├── Images/
│   ├── hardware_setup.jpg
│   ├── dashboard_load_on.png
│   └── dashboard_load_off.png
└── LICENSE
```

## Setup Instructions

1. Install the Arduino IDE.
2. Install the ESP32 board package.
3. Install the required libraries:
   - Adafruit IO Arduino
   - Adafruit INA219
4. Configure the Wi-Fi credentials and Adafruit IO authentication details in the source code.
5. Connect the INA219 sensor to the ESP32 using the I²C interface.
6. Upload the Arduino sketch to the ESP32.
7. Configure the required Adafruit IO feeds.
8. Open the Adafruit IO dashboard to monitor the readings.

## Future Enhancements

- ESP32 deep-sleep implementation for reduced power consumption.
- Comparator-based external wake-up when the load becomes active.
- Burst sampling to improve measurement efficiency.
- Sensor power gating to reduce standby power consumption.
- AWS IoT Core integration for scalable cloud connectivity and data storage.

## Author

**Mihit Manoj**  
B.Tech – Electrical and Computer Engineering  
Amrita Vishwa Vidyapeetham

---
