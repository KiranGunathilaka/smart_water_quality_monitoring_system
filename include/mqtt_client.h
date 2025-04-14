#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>  // Changed from WiFiClient to WiFiClientSecure
#include <PubSubClient.h>
#include "config.h"

class MQTTClient {
private:
    WiFiClientSecure espClient;  // Changed from WiFiClient to WiFiClientSecure
    PubSubClient client;
    
    unsigned long lastReconnectAttempt = 0;
    const unsigned long reconnectInterval = 5000; // 5 seconds between reconnect attempts
    
    char deviceId[20];
    
    // Buffer for JSON messages
    char jsonBuffer[256];
    
    // Callback function for incoming messages
    static void callback(char* topic, byte* payload, unsigned int length) {
        // Convert payload to string for easier handling
        char message[length + 1];
        for (unsigned int i = 0; i < length; i++) {
            message[i] = (char)payload[i];
        }
        message[length] = '\0';
        
        Serial.print("Message received on topic: ");
        Serial.print(topic);
        Serial.print(", Message: ");
        Serial.println(message);
        
        // Handle incoming messages here if needed
    }
    
    // Attempt to reconnect to MQTT broker
    bool reconnect() {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi not connected. Cannot reconnect to MQTT.");
            return false;
        }
        
        Serial.print("Attempting MQTT connection...");
        
        // Create a random client ID
        if (client.connect(deviceId, MQTT_USERNAME, MQTT_PASSWORD)) {
            Serial.println("connected");
            
            // Subscribe to any necessary topics
            client.subscribe(MQTT_COMMAND_TOPIC);
            
            return true;
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            return false;
        }
    }

public:
    MQTTClient() : client(espClient) {
        // Create a unique device ID using ESP's MAC address
        uint8_t mac[6];
        WiFi.macAddress(mac);
        sprintf(deviceId, "ESP32_%02X%02X%02X", mac[3], mac[4], mac[5]);
    }
    
    void init() {
        // Skip certificate verification (only for testing/development)
        espClient.setInsecure();
        
        // Set server and port
        client.setServer(MQTT_BROKER, MQTT_PORT);
        
        // Set callback function for incoming messages
        client.setCallback(callback);
        
        Serial.println("MQTT client initialized with TLS support");
    }
    
    void connect() {
        if (!client.connected()) {
            long now = millis();
            if (now - lastReconnectAttempt > reconnectInterval) {
                lastReconnectAttempt = now;
                if (reconnect()) {
                    lastReconnectAttempt = 0;
                }
            }
        }
    }
    
    void loop() {
        if (!client.connected()) {
            connect();
        }
        
        if (client.connected()) {
            client.loop();
        }
    }
    
    bool isConnected() {
        return client.connected();
    }
    
    // Send water quality data via MQTT
    bool publishWaterData(float ph, float tds, float turbidity, float temperature) {
        if (!client.connected()) {
            return false;
        }
        
        // Create JSON message
        snprintf(jsonBuffer, sizeof(jsonBuffer),
                "{\"deviceId\":\"%s\",\"timestamp\":%lu,\"ph\":%.2f,\"tds\":%.1f,\"turbidity\":%.1f,\"temperature\":%.1f}",
                deviceId, millis(), ph, tds, turbidity, temperature);
        
        // Publish to water quality topic
        return client.publish(MQTT_DATA_TOPIC, jsonBuffer, true);
    }
};

#endif // MQTT_CLIENT_H