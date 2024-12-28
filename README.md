# Smart Irrigation System  

## Overview  
The **Smart Irrigation System** is an IoT-based project designed to automate and optimize the irrigation process while maintaining a suitable environment for plants. This system uses multiple sensors and actuators to monitor and control soil moisture, temperature, and humidity. It provides real-time updates through a web application and uploads data to the cloud for analytics.  

## Features  
- **Automated Irrigation**:  
  - Monitors soil moisture levels and activates a pump to water plants when needed.  
- **Environmental Control**:  
  - Activates a fan to cool down the environment if the temperature is too high.  
  - Turns on a heater to prevent frost damage during low temperatures.  
- **Web Application**:  
  - Displays real-time status of:  
    - Soil moisture percentage.  
    - Temperature and humidity levels.  
    - Pump Status.  
- **Cloud Integration**:  
  - Data is uploaded to **ThingSpeak** for historical analysis and visualization.  

## Technology Stack  
- **Hardware**:  
  - ESP32 (microcontroller)  
  - DHT11 Sensor (temperature and humidity)  
  - Soil Moisture Sensor  
  - Pump, Fan, and Heater  
- **Software**:  
  - Web Application for real-time monitoring  
  - ThingSpeak for cloud data analytics  

## Functionality  
1. The system monitors environmental conditions using IoT sensors.  
2. Based on predefined thresholds:  
   - Activates the pump when soil moisture is below the threshold.  
   - Triggers the fan to cool the surroundings if the temperature is high.  
   - Turns on the heater to maintain a suitable temperature during colder conditions.  
3. Displays real-time data on the web interface.  
4. Uploads sensor data to ThingSpeak for advanced insights.  

## Installation and Setup  
### Hardware Setup:  
1. Connect the sensors (DHT11 and soil moisture) to the ESP32 microcontroller.  
2. Attach the pump, fan, and heater to their respective output pins.  
3. Ensure a stable power supply for all components.  

### Code Setup  
1. The project file contains fully commented code to help you set up the system step-by-step.  
2. Follow these steps to configure the code:

**Code Setup**

- Open the code in the Arduino IDE or your preferred editor.
- Update the following sections in the code:
- Wi-Fi Credentials: Replace with your Wi-Fi SSID and password.
- ThingSpeak API Key: Replace with your ThingSpeak Write API key.
- Threshold Values: Adjust the thresholds for soil moisture, temperature, and humidity as per your requirements.

3. **Upload Code**:
Connect your ESP32 to your computer and upload the code using the Arduino IDE.



