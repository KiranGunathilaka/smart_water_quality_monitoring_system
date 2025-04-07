#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
#define PH_PIN 4
#define TDS_PIN 15
#define TURBIDITY_PIN 5
#define TEMP_PIN 18

// ADC settings
#define VOLTAGE_REF 3.3f
#define ADC_RESOLUTION 4095.0f

// Timing constants
#define SAMPLE_INTERVAL 1000   // Sample interval in ms
#define MQTT_PUBLISH_INTERVAL 10000  // Publish to MQTT every 10 seconds
#define SERIAL_BAUD_RATE 9600

// pH sensor calibration constants
#define PH_CALIBRATION_OFFSET 0.0f
#define PH_CALIBRATION_SLOPE -0.18f
#define PH_NEUTRAL_VOLTAGE 2.5f

// TDS sensor calibration constants
#define TDS_TEMPERATURE_COEFFICIENT 0.02f
#define TDS_CALIBRATION_FACTOR 0.5f  // Adjust based on calibration
#define TDS_DEFAULT_TEMPERATURE 25.0f

// Turbidity sensor calibration constants
#define TURBIDITY_CLEAR_VOLTAGE 2.5f
#define TURBIDITY_MUDDY_VOLTAGE 4.2f
#define TURBIDITY_CLEAR_NTU 0.0f
#define TURBIDITY_MUDDY_NTU 1000.0f

// Temperature sensor constants
#define TEMP_OFFSET 0.5f
#define TEMP_SCALE_FACTOR 0.1f

// WiFi Configuration
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

// MQTT Configuration
#define MQTT_BROKER "409d3402dbeb4499af4b08c7dd40dbc1.s1.eu.hivemq.cloud"  // MQTT broker address
#define MQTT_PORT 8883                  // MQTT broker port
#define MQTT_USERNAME "swqms_iot"  // MQTT username
#define MQTT_PASSWORD "Swqms123"  // MQTT password
#define MQTT_DATA_TOPIC "reservoir/water_quality/data"  // Topic for publishing sensor data
#define MQTT_COMMAND_TOPIC "reservoir/water_quality/commands"  // Topic for receiving commands

#endif // CONFIG_H