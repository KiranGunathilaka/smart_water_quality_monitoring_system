#include <Arduino.h>
#include "sensors.h"
#include "config.h"
#include "wifi_manager.h"
#include "mqtt_client.h"

// Create instances of our classes
WaterSensors waterSensors;
WiFiManager wifiManager;
MQTTClient mqttClient;

// Timer variables for non-blocking operation
unsigned long lastSampleTime = 0;
unsigned long lastPublishTime = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  
  // Wait for serial connection to establish
  delay(100);
  
  // Print welcome message
  Serial.println();
  Serial.println("=======================================");
  Serial.println(" Water Quality Monitoring System v1.0");
  Serial.println("=======================================");
  
  // Initialize sensors
  waterSensors.init();
  Serial.println("Sensors initialized.");
  
  // Initialize WiFi
  wifiManager.init();
  
  // Initialize MQTT client
  mqttClient.init();
  
  Serial.println("System initialization complete. Starting measurements...");
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle WiFi connection
  wifiManager.wifi_reconnect();
  
  // Handle MQTT connection if WiFi is connected
  if (wifiManager.isConnected()) {
    mqttClient.loop();
  }
  
  // Check if it's time to take another sample
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
    // Update last sample time
    lastSampleTime = currentTime;
    
    // Read all sensors
    waterSensors.readSensors();
    
    // Print the results to serial
    waterSensors.printReadings();
  }
  
  // Check if it's time to publish data to MQTT
  if (currentTime - lastPublishTime >= MQTT_PUBLISH_INTERVAL) {
    // Update last publish time
    lastPublishTime = currentTime;
    
    // If we have a connection to MQTT broker, publish the data
    if (wifiManager.isConnected() && mqttClient.isConnected()) {
      // Get current sensor values
      float ph = waterSensors.getPH();
      float tds = waterSensors.getTDS();
      float turbidity = waterSensors.getTurbidity();
      float temperature = waterSensors.getTemperature();
      
      // Publish to MQTT
      bool published = mqttClient.publishWaterData(ph, tds, turbidity, temperature);
      
      if (published) {
        Serial.println("Data successfully published to MQTT broker");
      } else {
        Serial.println("Failed to publish data to MQTT broker");
      }
    } else if (!wifiManager.isConnected()) {
      Serial.println("Cannot publish data: WiFi not connected");
    } else {
      Serial.println("Cannot publish data: MQTT not connected");
    }
  }
  
  // Optional: Check for serial commands
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    // Process commands (example)
    switch (command) {
      case 'r': // For 'read now'
        waterSensors.readSensors();
        waterSensors.printReadings();
        break;
      case 'p': // For 'publish now'
        if (wifiManager.isConnected() && mqttClient.isConnected()) {
          bool published = mqttClient.publishWaterData(
            waterSensors.getPH(),
            waterSensors.getTDS(),
            waterSensors.getTurbidity(),
            waterSensors.getTemperature()
          );
          
          if (published) {
            Serial.println("Data manually published to MQTT broker");
          } else {
            Serial.println("Failed to manually publish data to MQTT broker");
          }
        } else {
          Serial.println("Cannot publish: WiFi or MQTT not connected");
        }
        break;
      default:
        // Ignore other characters
        break;
    }

    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}