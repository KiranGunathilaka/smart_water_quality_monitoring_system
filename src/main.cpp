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

void setup()

{
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

  // Synchronize time via NTP
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");  

  Serial.println("Waiting for NTP time sync...");
  while (time(nullptr) < 100000) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Time synchronized.");

  // Initialize MQTT client
  mqttClient.init();

  Serial.println("System initialization complete. Starting measurements...");
  Serial.println();
}

void loop()
{
  unsigned long currentTime = millis();

  // Handle WiFi connection
  wifiManager.wifi_reconnect();

  // Handle MQTT connection if WiFi is connected
  if (wifiManager.isConnected())
  {
    mqttClient.loop();
  }

  // Check if it's time to take another sample
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL)
  {
    // Update last sample time
    lastSampleTime = currentTime;

    // Read all sensors
    waterSensors.readSensors();

    // Print the results to serial
    waterSensors.printReadings();
  }

  // Check if it's time to publish data to MQTT
  if (currentTime - lastPublishTime >= MQTT_PUBLISH_INTERVAL)
  {
    // Update last publish time
    lastPublishTime = currentTime;

    // If we have a connection to MQTT broker, publish the data
    if (wifiManager.isConnected() && mqttClient.isConnected())
    {
      time_t now = time(nullptr);
      now += 19800; // Add 5 hours 30 minutes in seconds (5*3600 + 30*60 = 19800)

      struct tm *timInf = gmtime(&now); // Convert adjusted time to struct tm

      char timeStr[30];
      // Format: YYYY-MM-DDTHH:MM:SS.000+00:00        (eg: 2025-04-15T17:24:56.000+00:00 )
      snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02dT%02d:%02d:%02d.000+00:00",
               timInf->tm_year + 1900, timInf->tm_mon + 1, timInf->tm_mday,
               timInf->tm_hour, timInf->tm_min, timInf->tm_sec);

      String timeString = String(timeStr);

      // Get current sensor values
      float ph = waterSensors.getPH();
      float tds = waterSensors.getTDS();
      float turbidity = waterSensors.getTurbidity();
      float temperature = waterSensors.getTemperature();

      // Publish to MQTT
      bool published = mqttClient.publishWaterData(timeString, ph, tds, turbidity, temperature);

      if (published)
      {
        Serial.println("Data successfully published to MQTT broker");
      }
      else
      {
        Serial.println("Failed to publish data to MQTT broker");
      }
    }
    else if (!wifiManager.isConnected())
    {
      Serial.println("Cannot publish data: WiFi not connected");
    }
    else
    {
      Serial.println("Cannot publish data: MQTT not connected");
    }
  }
}