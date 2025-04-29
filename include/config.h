#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
#define PH_PIN 34
#define TDS_PIN 33
#define TURBIDITY_PIN 35
#define TEMP_PIN 32         

// ADC settings
#define VOLTAGE_REF 3.3f    
#define ADC_RESOLUTION 4095.0f

// Timing constants
#define SAMPLE_INTERVAL 1000   // Sample interval in ms
#define MQTT_PUBLISH_INTERVAL 5000  // Publish to MQTT every 5 seconds
#define SERIAL_BAUD_RATE 9600

// pH sensor calibration constants
#define PH_CALIBRATION_SLOPE -0.22f   // 3.3 * (3850/4095) / (14-0)   3850 is the maximum adc value practically reaching
#define PH_7_VOLTAGE 1.58f
static const int MAX_PH_WINDOW_SIZE = 10; // Maximum possible window size

// TDS sensor calibration constants
#define TDS_TEMPERATURE_COEFFICIENT 0.02f
#define TDS_CALIBRATION_FACTOR 0.5f  
#define TDS_DEFAULT_TEMPERATURE 25.0f

// Turbidity sensor calibration constsnts (Didn't able to deduce anything, whatever the Turbidity, output voltage < 0.2V)
#define TURBIDITY_CLEAR_VOLTAGE 0.0f 
#define TURBIDITY_MUDDY_VOLTAGE 3.3f
#define TURBIDITY_CLEAR_NTU 0.0f
#define TURBIDITY_MUDDY_NTU 50.0f

// Temperature sensor constants
#define TEMP_OFFSET 0.5f
#define TEMP_SCALE_FACTOR 0.1f
#define TEMP_30_VOLTAGE 0.6  // Averaged for 10 Values (Very high variation (from about 80 to 100 including outliers in between 65 - 75 for about every 10 reading))
#define TEMP_45_VOLTAGE 0.84 // Same very high variation

// WiFi Configuration
#define WIFI_SSID "Chiki Chiki Bamba"
#define WIFI_PASSWORD "DiscreteFourierTransform"

// MQTT Configuration
#define MQTT_BROKER "5df16d8a5a1c438294a51cb556f6df87.s1.eu.hivemq.cloud"  
#define MQTT_PORT 8883               
#define MQTT_USERNAME "swqms_iot"  
#define MQTT_PASSWORD "Swqms123"  
#define MQTT_DATA_TOPIC "reservoir/water_quality/data"  // Topic for publishing sensor data
#define MQTT_COMMAND_TOPIC "reservoir/water_quality/commands"  // Topic for receiving commands

#endif // CONFIG_H