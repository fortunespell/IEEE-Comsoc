# IEEE-Comsoc
This repository contains the codebase and design documentation for an innovative IoT healthcare monitoring system developed using ESP32 and Arduino. The system enables real-time transmission of SpO2, heart rate, and blood pressure values to a web interface, facilitating remote patient monitoring and healthcare analytics.
# Team Engineers Health Monitor

This project is an ESP32-based health monitoring system that measures heart rate, SpO2, and ECG values. It also provides a web interface to display these measurements in real-time and uses MQTT to publish the data to an MQTT broker.

## Components Used
- ESP32
- MAX30100 Pulse Oximeter Sensor
- ECG Sensor
- WiFi Module (integrated in ESP32)
- LED (onboard or external)

## Libraries Required
- WiFi.h
- WebServer.h
- Wire.h
- PubSubClient.h
- MAX30100_PulseOximeter.h
- Chart.js (for the web interface)

## Features
- Real-time monitoring of heart rate, SpO2, and ECG values
- Web interface to display the measurements using charts
- MQTT integration to publish data to an MQTT broker
- Emergency alert if the measurements exceed predefined thresholds

## Setup Instructions

### Hardware Setup
1. Connect the MAX30100 Pulse Oximeter sensor to the ESP32.
2. Connect the ECG sensor to the ESP32.
3. Optionally, connect an external LED to GPIO pin 2 (or use the onboard LED).

### Software Setup
1. **Install Arduino IDE**: Ensure you have the Arduino IDE installed on your computer.
2. **Add ESP32 Board to Arduino IDE**: Follow [these instructions](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/) to add the ESP32 board to the Arduino IDE.
3. **Install Required Libraries**:
   - Open Arduino IDE, go to `Sketch` > `Include Library` > `Manage Libraries`.
   - Install the following libraries:
     - WiFi
     - WebServer
     - PubSubClient
     - MAX30100_PulseOximeter
4. **Upload the Code**:
   - Copy and paste the provided code into a new Arduino sketch.
   - Modify the `ssid` and `password` variables with your WiFi credentials.
   - Modify the `mqtt_server`, `mqtt_topic_heart_rate`, and `mqtt_topic_spo2` variables as needed.
   - Upload the code to the ESP32.

### Web Interface
- Access the web interface by navigating to the IP address of the ESP32 in a web browser (displayed in the Serial Monitor after the ESP32 connects to WiFi).

## Code Explanation

### Setup Function
- Connects to the WiFi network.
- Initializes the web server and MQTT client.
- Initializes the Pulse Oximeter sensor.
- Generates random data for heart rate, SpO2, and ECG for testing purposes.

### Loop Function
- Reconnects to the MQTT broker if disconnected.
- Handles web server requests.
- Updates Pulse Oximeter readings.
- Publishes heart rate and SpO2 data to MQTT topics every second.

### Web Server Handlers
- `/`: Serves the main webpage with the charts.
- `/data`: Sends the latest sensor data in JSON format.

### JavaScript Code
- Uses Chart.js to display real-time heart rate, SpO2, and ECG values.
- Fetches data from the `/data` endpoint every second and updates the charts.

## Thresholds and Alerts
- Predefined thresholds for heart rate, SpO2, and ECG values are used to trigger alerts.
- If the measurements exceed these thresholds, an emergency popup message can be triggered (to be implemented).

---

## Troubleshooting
- If the web interface is not accessible, ensure the ESP32 is connected to the WiFi network.
- Check the Serial Monitor for any error messages.
- Verify connections to the sensors.

## Future Improvements
- Implement the emergency popup message functionality in the web interface.
- Add data logging to store historical measurements.
- Integrate with a cloud service for remote monitoring.
