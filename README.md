# napkin-shredder
Automated napkin shredding system using ESP32 with heater control, shredder motor, temperature monitoring, rotary encoder interface, and LCD status display.
# Smart Napkin Shredder Machine (ESP32)

## Overview

This project is an **automated napkin shredding machine** developed using **ESP32**.  
The system performs heating and shredding operations in a controlled sequence while monitoring temperature and machine position using sensors and limit switches.

A **rotary encoder interface with LCD display** allows the user to configure process parameters such as heating and shredding time.

The system is designed for **safe automated operation with homing control, temperature monitoring, and relay-based actuator control**.

---

## Features

- ESP32 based automation
- Heater temperature monitoring using **DS18B20**
- LCD user interface
- Rotary encoder for parameter adjustment
- Configurable heating and shredding duration
- Automated process control
- Homing system using limit switches
- Safety stop for relays
- Buzzer alerts for process completion
- Non-blocking state machine control

---

## Hardware Components

- ESP32
- DS18B20 Temperature Sensor
- 16x2 I2C LCD Display
- Rotary Encoder (CLK, DT, SW)
- 8 Relay Modules
- Heater
- Shredder Motor
- Limit Switches
- Buzzer
- Power Supply

---

## System Operation

### 1. Parameter Setup
The user sets the following parameters using the **rotary encoder**:

- Heater duration
- Shredder duration

These values are displayed on the **LCD screen**.

### 2. Start Process
Press and hold the encoder button to start the machine.

### 3. Heating Phase
The system activates the heater relay and maintains heating for the selected duration.

Temperature is continuously monitored using the **DS18B20 sensor**.

### 4. Shredding Phase
After heating completes:

- The shredder motor is activated
- Napkins are shredded for the selected duration

### 5. Position Control
Limit switches control machine movement between:

- Top position
- Middle position
- Bottom position

### 6. Homing
After the process is complete, the machine automatically returns to the **home position**.

### 7. Process Completion
- LCD displays **PROCESS DONE**
- Buzzer alerts the user

---

## Hardware Pin Configuration

| Component | ESP32 Pin |
|----------|-----------|
| Relay 1 | 25 |
| Relay 2 | 2 |
| Relay 3 | 4 |
| Relay 4 | 16 |
| Relay 5 | 17 |
| Relay 6 | 26 |
| Heater Relay | 18 |
| Shredder Motor Relay | 19 |
| Limit Switch 1 | 13 |
| Limit Switch 2 | 12 |
| Limit Switch 3 | 27 |
| Limit Switch 4 | 14 |
| Limit Switch 5 | 5 |
| Limit Switch 6 | 15 |
| Buzzer | 23 |
| Encoder CLK | 32 |
| Encoder DT | 33 |
| Encoder SW | 34 |
| Temperature Sensor | 35 |

---

## User Interface

### LCD Display
The LCD shows:

- Heater time
- Shredder time
- Remaining process time
- Temperature

### Rotary Encoder Controls

| Action | Function |
|------|---------|
| Rotate | Adjust time |
| Short Press | Switch between heater and shredder settings |
| Long Press | Start process |


---

## Safety Features

- Homing verification
- Relay shutdown protection
- Limit switch confirmation delay
- Temperature monitoring
- Emergency stop capability

---

## Future Improvements

- IoT monitoring
- Remote control via WiFi
- Over-temperature protection
- Motor current monitoring
- Mobile app interface

---

## Author

Embedded Systems Project developed using **ESP32 firmware and hardware automation design**.
