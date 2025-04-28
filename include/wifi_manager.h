#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

class WiFiManager {
private:
    unsigned long lastWiFiCheckTime = 0;
    const unsigned long wifiCheckInterval = 30000; // 30 seconds between WiFi checks
    bool wasConnected = false;
    
public:
    void init() {
        Serial.println("Connecting to WiFi...");
        
        // Connect to WiFi network
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        // Wait for connection (with timeout)
        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000) {
            delay(500);
            Serial.print(".");
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println();
            Serial.print("Connected to WiFi network. IP address: ");
            Serial.println(WiFi.localIP());
            wasConnected = true;
        } else {
            Serial.println();
            Serial.println("Failed to connect to WiFi. Will retry later.");
        }
    }
    
    void wifi_reconnect() {
        unsigned long currentTime = millis();
        
        // Check WiFi connection periodically
        if (currentTime - lastWiFiCheckTime > wifiCheckInterval) {
            lastWiFiCheckTime = currentTime;
            
            if (WiFi.status() != WL_CONNECTED) {
                if (wasConnected) {
                    Serial.println("WiFi connection lost. Reconnecting...");
                    wasConnected = false;
                }
                
                WiFi.reconnect();
            } else if (!wasConnected) {
                Serial.println("WiFi reconnected!");
                Serial.print("IP Address: ");
                Serial.println(WiFi.localIP());
                wasConnected = true;
            }
        }
    }
    
    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
};

#endif 