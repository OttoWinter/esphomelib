#ifndef ESPHOME_DEFINES_H
#define ESPHOME_DEFINES_H

#ifdef ARDUINO_ARCH_ESP8266
#include <core_version.h>
#endif

#define ESPHOME_VERSION "1.13.0-dev"

#define HOT __attribute__((hot))
#define ESPDEPRECATED(msg) __attribute__((deprecated(msg)))
#define ALWAYS_INLINE __attribute__((always_inline))

#ifndef DOXYGEN
#define ESPHOME_NAMESPACE_BEGIN namespace esphome {
#define ESPHOME_NAMESPACE_END }
#define ESPHOME_NAMESPACE esphome
#else
#define ESPHOME_NAMESPACE_BEGIN / ## /
#define ESPHOME_NAMESPACE_END / ## /
#define ESPHOME_NAMESPACE
#endif

#ifndef ESPHOME_USE
#define USE_OTA
#define USE_I2C
#define USE_BINARY_SENSOR
#define USE_GPIO_BINARY_SENSOR
#define USE_STATUS_BINARY_SENSOR
#define USE_SENSOR
#define USE_DHT_SENSOR
#define USE_DHT12_SENSOR
#define USE_DALLAS_SENSOR
#define USE_PULSE_COUNTER_SENSOR
#define USE_ADC_SENSOR
#define USE_ADS1115_SENSOR
#define USE_BMP085_SENSOR
#define USE_HTU21D_SENSOR
#define USE_HDC1080_SENSOR
#define USE_ULTRASONIC_SENSOR
#define USE_WIFI_SIGNAL_SENSOR
#define USE_OUTPUT
#ifdef ARDUINO_ARCH_ESP32
#define USE_LEDC_OUTPUT
#endif
#define USE_PCA9685_OUTPUT
#define USE_GPIO_OUTPUT
#ifdef ARDUINO_ARCH_ESP8266
#ifndef ARDUINO_ESP8266_RELEASE_2_3_0
#define USE_ESP8266_PWM_OUTPUT
#endif
#endif
#define USE_LIGHT
#define USE_SWITCH
#define USE_OUTPUT_SWITCH
#define USE_REMOTE
#define USE_REMOTE_RECEIVER
#define USE_REMOTE_TRANSMITTER
#define USE_GPIO_SWITCH
#define USE_RESTART_SWITCH
#define USE_SHUTDOWN_SWITCH
#define USE_FAN
#define USE_DEBUG_COMPONENT
#define USE_DEEP_SLEEP
#define USE_PCF8574
#define USE_MCP23017
#define USE_IO
#define USE_SDS011
#define USE_MPU6050
#define USE_TSL2561
#define USE_BH1750
#define USE_BME280
#define USE_BMP280
#define USE_BME680
#define USE_SHT3XD
#define USE_TEMPLATE_SENSOR
#ifdef ARDUINO_ARCH_ESP32
#define USE_ESP32_TOUCH_BINARY_SENSOR
#endif
#ifdef ARDUINO_ARCH_ESP32
#define USE_ESP32_BLE_TRACKER
#define USE_ESP32_BLE_BEACON
#endif
#define USE_ROTARY_ENCODER_SENSOR
#define USE_MAX31855_SENSOR
#define USE_MAX6675_SENSOR
#define USE_TEMPLATE_BINARY_SENSOR
#define USE_TEMPLATE_SWITCH
#define USE_COVER
#define USE_TEMPLATE_COVER
#ifdef ARDUINO_ARCH_ESP32
#define USE_ESP32_HALL_SENSOR
#define USE_ESP32_CAMERA
#endif
#define USE_DUTY_CYCLE_SENSOR
#define USE_STATUS_LED
#define USE_SPI
#define USE_PN532
#define USE_UART
#define USE_MHZ19
#define USE_UART_SWITCH
#define USE_UPTIME_SENSOR
#define USE_INA219
#define USE_INA3221
#define USE_HMC5883L
#define USE_RDM6300
#define USE_MS5611
#define USE_HX711
#define USE_TCS34725
#define USE_MAX7219
#define USE_LCD_DISPLAY
#define USE_LCD_DISPLAY_PCF8574
#define USE_SSD1306
#define USE_WAVESHARE_EPAPER
#define USE_DISPLAY
#define USE_TIME
#define USE_SNTP_COMPONENT
#define USE_NEXTION
#define USE_HLW8012
#define USE_TEXT_SENSOR
#define USE_MQTT_SUBSCRIBE_TEXT_SENSOR
#define USE_VERSION_TEXT_SENSOR
#define USE_TEMPLATE_TEXT_SENSOR
#define USE_MQTT_SUBSCRIBE_SENSOR
#define USE_CSE7766
#define USE_PMSX003
#define USE_ENDSTOP_COVER
#define USE_TIME_BASED_COVER
#define USE_STEPPER
#define USE_A4988
#define USE_ULN2003
#define USE_TOTAL_DAILY_ENERGY_SENSOR
#define USE_MY9231_OUTPUT
#define USE_SM16716_OUTPUT
#define USE_CUSTOM_SENSOR
#define USE_CUSTOM_BINARY_SENSOR
#define USE_CUSTOM_OUTPUT
#define USE_CUSTOM_SWITCH
#define USE_CUSTOM_TEXT_SENSOR
#define USE_CUSTOM_COMPONENT
#define USE_API
#define USE_HOMEASSISTANT_TIME
#define USE_HOMEASSISTANT_SENSOR
#define USE_HOMEASSISTANT_TEXT_SENSOR
#define USE_HOMEASSISTANT_BINARY_SENSOR
#define USE_APDS9960
#define USE_MPR121
#define USE_MQTT
#define USE_COPY_OUTPUT
#define USE_WIFI_INFO_TEXT_SENSOR
#define USE_SERVO
#define USE_TTP229_LSF
#define USE_CLIMATE
#define USE_BANG_BANG_CLIMATE
#endif

#ifdef USE_REMOTE_RECEIVER
#ifndef USE_REMOTE
#define USE_REMOTE
#endif
#ifndef USE_BINARY_SENSOR
#define USE_BINARY_SENSOR
#endif
#endif
#ifdef USE_REMOTE_TRANSMITTER
#ifndef USE_REMOTE
#define USE_REMOTE
#endif
#ifndef USE_SWITCH
#define USE_SWITCH
#endif
#endif
#ifdef USE_ESP32_TOUCH_BINARY_SENSOR
#ifndef USE_BINARY_SENSOR
#define USE_BINARY_SENSOR
#endif
#endif
#ifdef USE_ESP32_BLE_TRACKER
#ifndef USE_BINARY_SENSOR
#define USE_BINARY_SENSOR
#endif
#ifndef USE_SENSOR
#define USE_SENSOR
#endif
#endif
#ifdef USE_PN532
#ifndef USE_BINARY_SENSOR
#define USE_BINARY_SENSOR
#endif
#endif
#ifdef USE_RDM6300
#ifndef USE_BINARY_SENSOR
#define USE_BINARY_SENSOR
#endif
#endif
#ifdef USE_DALLAS_SENSOR
#ifndef USE_ONE_WIRE
#define USE_ONE_WIRE
#endif
#endif
#ifdef USE_LIGHT
#ifndef USE_OUTPUT
#define USE_OUTPUT
#endif
#endif
#ifdef USE_PCF8574
#ifndef USE_IO
#define USE_IO
#endif
#endif
#ifdef USE_MCP23017
#ifndef USE_IO
#define USE_IO
#endif
#endif
#ifdef USE_LCD_DISPLAY_PCF8574
#ifndef USE_LCD_DISPLAY
#define USE_LCD_DISPLAY
#endif
#endif
#ifdef USE_HLW8012
#ifndef USE_PULSE_COUNTER_SENSOR
#define USE_PULSE_COUNTER_SENSOR
#endif
#endif
#ifdef USE_MY9231_OUTPUT
#ifndef USE_OUTPUT
#define USE_OUTPUT
#endif
#endif

#ifdef USE_APDS9960
#ifndef USE_SENSOR
#define USE_SENSOR
#endif
#ifndef USE_BINARY_SENSOR
#define USE_BINARY_SENSOR
#endif
#endif

#ifdef USE_MPR121
#ifndef USE_BINARY_SENSOR
#define USE_BINARY_SENSOR
#endif
#endif

#if defined(USE_MQTT) && defined(USE_BINARY_SENSOR)
#ifndef USE_MQTT_BINARY_SENSOR
#define USE_MQTT_BINARY_SENSOR
#endif
#endif

#if defined(USE_MQTT) && defined(USE_COVER)
#ifndef USE_MQTT_COVER
#define USE_MQTT_COVER
#endif
#endif

#if defined(USE_MQTT) && defined(USE_FAN)
#ifndef USE_MQTT_FAN
#define USE_MQTT_FAN
#endif
#endif

#if defined(USE_MQTT) && defined(USE_LIGHT)
#ifndef USE_MQTT_LIGHT
#define USE_MQTT_LIGHT
#endif
#endif

#if defined(USE_MQTT) && defined(USE_CUSTOM_COMPONENT)
#ifndef USE_MQTT_CUSTOM_COMPONENT
#define USE_MQTT_CUSTOM_COMPONENT
#endif
#endif

#if defined(USE_MQTT) && defined(USE_SENSOR)
#ifndef USE_MQTT_SENSOR
#define USE_MQTT_SENSOR
#endif
#endif

#if defined(USE_MQTT) && defined(USE_SWITCH)
#ifndef USE_MQTT_SWITCH
#define USE_MQTT_SWITCH
#endif
#endif

#if defined(USE_MQTT) && defined(USE_TEXT_SENSOR)
#ifndef USE_MQTT_TEXT_SENSOR
#define USE_MQTT_TEXT_SENSOR
#endif
#endif

#if defined(USE_MQTT) && defined(USE_CLIMATE)
#ifndef USE_MQTT_CLIMATE
#define USE_MQTT_CLIMATE
#endif
#endif

#if !defined(DONT_STORE_LOG_STR_IN_FLASH) && defined(ARDUINO_ARCH_ESP8266)
#ifndef USE_STORE_LOG_STR_IN_FLASH
#define USE_STORE_LOG_STR_IN_FLASH
#endif
#endif

#endif  // ESPHOME_DEFINES_H
