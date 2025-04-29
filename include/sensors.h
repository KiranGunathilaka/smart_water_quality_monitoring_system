#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"
#include <Arduino.h>

class WaterSensors {
private:
    // Raw sensor values
    int phRawValue;
    int tdsRawValue;
    int turbidityRawValue;
    int tempRawValue;
    
    // Voltage values
    float phVoltage;
    float tdsVoltage;
    float turbidityVoltage;
    float tempVoltage;
    
    // Processed values
    float phValue;
    float tdsValue;     // in ppm (parts per million)
    float turbidityValue; // in NTU (Nephelometric Turbidity Units)
    float temperatureValue; // in Celsius
    
    // Moving average for pH readings
    static const int MAX_PH_WINDOW_SIZE = 10; // Maximum possible window size
    float phReadings[MAX_PH_WINDOW_SIZE];     // Array to store readings
    int phWindowSize;                         // Current window size
    int phReadingIndex;                       // Current index in the circular buffer
    
    // Convert raw ADC value to voltage
    float rawToVoltage(int rawValue) {
        return rawValue * (VOLTAGE_REF / ADC_RESOLUTION);
    }
    
    // Convert pH voltage to pH value
    float convertVoltageToPH(float voltage) {
        return 7.0f - ((voltage - PH_7_VOLTAGE) / PH_CALIBRATION_SLOPE);
    }
    
    // Calculate moving average for pH readings
    float calculatePhMovingAverage() {
        float sum = 0.0f;
        int count = 0;
        
        // Count how many valid readings we have
        for (int i = 0; i < phWindowSize; i++) {
            if (phReadings[i] != 0.0f || count > 0) { // Skip initial zeros unless we've already started recording
                sum += phReadings[i];
                count++;
            }
        }
        
        // Return average if we have readings, otherwise return 0
        return (count > 0) ? (sum / count) : 0.0f;
    }
    
    // Convert TDS voltage to TDS value (ppm)
    float convertVoltageToTDS(float voltage, float temperature) {
        // // Temperature compensation
        // float compensationCoefficient = 1.0f + TDS_TEMPERATURE_COEFFICIENT * (temperature - 25.0f);
        // float compensatedVoltage = voltage / compensationCoefficient;

        //for the moment temperature compensation is neglected as the temperature readings has higher variation
        //possible to implement this to manually measure the temperature and send that via the node red dashboard
        float compensatedVoltage = voltage;
        
        // Convert to TDS value (ppm)
        // https://randomnerdtutorials.com/arduino-tds-water-quality-sensor/
        float tds = (133.42f * compensatedVoltage * compensatedVoltage * compensatedVoltage - 
                    255.86f * compensatedVoltage * compensatedVoltage + 
                    857.39f * compensatedVoltage) * TDS_CALIBRATION_FACTOR;
        return tds;
    }
    
    // Convert turbidity voltage to NTU
    float convertVoltageToTurbidity(float voltage) {
        // Linear interpolation between clear water and muddy water
        if (voltage > VOLTAGE_REF) voltage = VOLTAGE_REF; // Safety check
        
        // Map voltage to NTU using linear interpolation
        return map(voltage, TURBIDITY_CLEAR_VOLTAGE, TURBIDITY_MUDDY_VOLTAGE, 
                   TURBIDITY_CLEAR_NTU, TURBIDITY_MUDDY_NTU);
    }
    
    // Helper function to map float values
    float map(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    
    // Assumed linear Conversion
    float convertVoltageToTemperature(float voltage) {
        return (voltage - TEMP_30_VOLTAGE) / (TEMP_45_VOLTAGE - TEMP_30_VOLTAGE) *15 + 30;
    }

public:
    WaterSensors() {
        phRawValue = tdsRawValue = turbidityRawValue = tempRawValue = 0;
        phVoltage = tdsVoltage = turbidityVoltage = tempVoltage = 0.0f;
        phValue = tdsValue = turbidityValue = temperatureValue = 0.0f;
        
        // Initialize moving average filter
        phWindowSize = 5; // Default window size
        phReadingIndex = 0;
        
        // Initialize the pH readings array
        for (int i = 0; i < MAX_PH_WINDOW_SIZE; i++) {
            phReadings[i] = 0.0f;
        }
    }

    void init() {
        // Configure input pins
        pinMode(PH_PIN, INPUT);
        pinMode(TDS_PIN, INPUT);
        pinMode(TURBIDITY_PIN, INPUT);
        pinMode(TEMP_PIN, INPUT);
    }
    
    // Set the window size for pH moving average filter
    void setPhWindowSize(int size) {
        if (size > 0 && size <= MAX_PH_WINDOW_SIZE) {
            phWindowSize = size;
            
            // Reset the array when changing window size
            for (int i = 0; i < MAX_PH_WINDOW_SIZE; i++) {
                phReadings[i] = 0.0f;
            }
            phReadingIndex = 0;
        }
    }
    
    // Get current window size
    int getPhWindowSize() {
        return phWindowSize;
    }
    
    void readSensors() {
        // Read raw values
        phRawValue = analogRead(PH_PIN);
        tdsRawValue = analogRead(TDS_PIN);
        turbidityRawValue = analogRead(TURBIDITY_PIN);
        tempRawValue = analogRead(TEMP_PIN);
        
        // Convert to voltages
        phVoltage = rawToVoltage(phRawValue);
        tdsVoltage = rawToVoltage(tdsRawValue);
        turbidityVoltage = rawToVoltage(turbidityRawValue);
        tempVoltage = rawToVoltage(tempRawValue);
        
        // Process voltages to actual values
        temperatureValue = convertVoltageToTemperature(tempVoltage);
        
        // Calculate pH value and add to moving average
        float currentPhValue = convertVoltageToPH(phVoltage);
        
        // Store the current pH reading in the array
        phReadings[phReadingIndex] = currentPhValue;
        phReadingIndex = (phReadingIndex + 1) % phWindowSize; // Increment index and wrap around
        
        // Calculate the moving average pH value
        phValue = calculatePhMovingAverage();
        
        // Process other sensor values
        tdsValue = convertVoltageToTDS(tdsVoltage, temperatureValue); // Use actual temperature for compensation
        turbidityValue = convertVoltageToTurbidity(turbidityVoltage);
    }
    
    // Get methods
    float getPH() { return phValue; }
    float getTDS() { return tdsValue; }
    float getTurbidity() { return turbidityValue; }
    float getTemperature() { return temperatureValue; }
    
    // Get voltage methods (for debugging/calibration)
    float getPHVoltage() { return phVoltage; }
    float getTDSVoltage() { return tdsVoltage; }
    float getTurbidityVoltage() { return turbidityVoltage; }
    float getTemperatureVoltage() { return tempVoltage; }
    
    // Get raw values methods (for debugging/calibration)
    int getPHRaw() { return phRawValue; }
    int getTDSRaw() { return tdsRawValue; }
    int getTurbidityRaw() { return turbidityRawValue; }
    int getTemperatureRaw() { return tempRawValue; }

    // Get the current pH value without moving average (for comparison)
    float getRawPH() { 
        return convertVoltageToPH(phVoltage); 
    }

    void printReadings() {
        
        Serial.print("Temperature: ");
        Serial.print(temperatureValue, 1);
        Serial.print(" °C (Raw: ");
        Serial.print(tempRawValue);
        Serial.print(", Voltage: ");
        Serial.print(tempVoltage, 2);
        Serial.print("V)");
        
        Serial.print("   pH: ");
        Serial.print(phValue, 2);  // Display pH with 2 decimal places
        Serial.print(" (Raw: ");
        Serial.print(phRawValue);
        Serial.print(", Voltage: ");
        Serial.print(phVoltage, 2);
        Serial.print("V)");
        
        Serial.print("   TDS: ");
        Serial.print(tdsValue, 1); // Display TDS with 1 decimal place instead of as integer
        Serial.print(" ppm (Raw: ");
        Serial.print(tdsRawValue);
        Serial.print(", Voltage: ");
        Serial.print(tdsVoltage, 2);
        Serial.print("V)");
        
        Serial.print("   Turbidity: ");
        Serial.print(turbidityValue, 2);  // Display turbidity with 2 decimal places
        Serial.print(" NTU (Raw: ");
        Serial.print(turbidityRawValue);
        Serial.print(", Voltage: ");
        Serial.print(turbidityVoltage, 2);
        Serial.println("V)");
    }
};

#endif