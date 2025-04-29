#include <Arduino.h>
#include "sensors.h"
#include "config.h"
#include "wifi_manager.h"
#include "mqtt_client.h"

WaterSensors waterSensors;
WiFiManager wifiManager;
MQTTClient mqttClient;

// Variables for timing
unsigned long lastSampleTime = 0;
unsigned long lastPublishTime = 0;

// Variables for averaging samples
const int NUM_SAMPLES = 5;
int currentSampleCount = 0;

// Arrays to store multiple readings
float phReadings[NUM_SAMPLES] = {0};
float tdsReadings[NUM_SAMPLES] = {0};
float turbidityReadings[NUM_SAMPLES] = {0};
float temperatureReadings[NUM_SAMPLES] = {0};

// Variables to store averages
float avgPh = 0;
float avgTds = 0;
float avgTurbidity = 0;
float avgTemperature = 0;

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println();
  Serial.println("=======================================");
  Serial.println(" Water Quality Monitoring System v1.0");
  Serial.println("=======================================");
  
  waterSensors.init();
  Serial.println("Sensors initialized.");
  
  wifiManager.init();
  
  // Synchronize time via NTP with ESP32s RTC
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for NTP time sync...");
  while (time(nullptr) < 100000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Time synchronized.");
  
  mqttClient.init();
  Serial.println("System initialization complete. Starting measurements...");
  Serial.println();
}

// Function to calculate averages from collected samples
void calculateAverages() {
  float sumPh = 0, sumTds = 0, sumTurbidity = 0, sumTemperature = 0;
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sumPh += phReadings[i];
    sumTds += tdsReadings[i];
    sumTurbidity += turbidityReadings[i];
    sumTemperature += temperatureReadings[i];
  }
  
  avgPh = sumPh / NUM_SAMPLES;
  avgTds = sumTds / NUM_SAMPLES;
  avgTurbidity = sumTurbidity / NUM_SAMPLES;
  avgTemperature = sumTemperature / NUM_SAMPLES;
  
  Serial.println("===== AVERAGE READINGS =====");
  Serial.print("pH: ");
  Serial.println(avgPh, 2);
  Serial.print("TDS: ");
  Serial.println(avgTds, 2);
  Serial.print("Turbidity: ");
  Serial.println(avgTurbidity, 2);
  Serial.print("Temperature: ");
  Serial.println(avgTemperature, 2);
  Serial.println("===========================");
}

void loop()
{
  unsigned long currentTime = millis();
  
  wifiManager.wifi_reconnect();
  if (wifiManager.isConnected()) {
    mqttClient.loop();
  }
  
  // Check if it's time to take another sample
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
    lastSampleTime = currentTime;
    
    // Take new readings
    waterSensors.readSensors();
    
    // Store the readings in the arrays
    phReadings[currentSampleCount] = waterSensors.getPH();
    tdsReadings[currentSampleCount] = waterSensors.getTDS();
    turbidityReadings[currentSampleCount] = waterSensors.getTurbidity();
    temperatureReadings[currentSampleCount] = waterSensors.getTemperature();
    
    // Print the individual readings
    Serial.print("Sample #");
    Serial.print(currentSampleCount + 1);
    Serial.print("   readings:");
    waterSensors.printReadings();
    
    // Increment sample count and wrap around if necessary
    currentSampleCount++;
    if (currentSampleCount >= NUM_SAMPLES) {
      currentSampleCount = 0;
    }
  }
  
  // Check if it's time to publish data to MQTT
  if (currentTime - lastPublishTime >= MQTT_PUBLISH_INTERVAL) {
    lastPublishTime = currentTime;
    
    // Calculate averages from the collected samples
    calculateAverages();
    
    if (wifiManager.isConnected() && mqttClient.isConnected()) {
      time_t now = time(nullptr);
      now += 19800; // Add 5 hours 30 minutes in seconds (5*3600 + 30*60 = 19800)
      struct tm *timInf = gmtime(&now); // Convert adjusted time to struct tm
      char timeStr[30];
      
      // Format: YYYY-MM-DDTHH:MM:SS.000+00:00 (eg: 2025-04-15T17:24:56.000+00:00 )
      snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02dT%02d:%02d:%02d.000+00:00",
               timInf->tm_year + 1900, timInf->tm_mon + 1, timInf->tm_mday,
               timInf->tm_hour, timInf->tm_min, timInf->tm_sec);
      String timeString = String(timeStr);
      
      // Publish the average values to MQTT
      bool published = mqttClient.publishWaterData(timeString, avgPh, avgTds, avgTurbidity, avgTemperature);
      
      if (published) {
        Serial.println("Average data successfully published to MQTT broker");
      } else {
        Serial.println("Failed to publish average data to MQTT broker");
      }
    } else if (!wifiManager.isConnected()) {
      Serial.println("Cannot publish data: WiFi not connected");
    } else {
      Serial.println("Cannot publish data: MQTT not connected");
    }
  }
}