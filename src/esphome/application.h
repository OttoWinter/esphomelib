#ifndef ESPHOME_APPLICATION_H
#define ESPHOME_APPLICATION_H

#include <vector>
#include "esphome/defines.h"
#include "esphome/api/api_server.h"
#include "esphome/automation.h"
#include "esphome/component.h"
#include "esphome/controller.h"
#include "esphome/custom_component.h"
#include "esphome/debug_component.h"
#include "esphome/deep_sleep_component.h"
#include "esphome/esp32_ble_beacon.h"
#include "esphome/esp32_ble_tracker.h"
#include "esphome/esp_one_wire.h"
#include "esphome/esphal.h"
#include "esphome/esppreferences.h"
#include "esphome/ethernet_component.h"
#include "esphome/i2c_component.h"
#include "esphome/log.h"
#include "esphome/log_component.h"
#include "esphome/ota_component.h"
#include "esphome/power_supply_component.h"
#include "esphome/servo.h"
#include "esphome/spi_component.h"
#include "esphome/status_led.h"
#include "esphome/uart_component.h"
#include "esphome/web_server.h"
#include "esphome/wifi_component.h"
#include "esphome/binary_sensor/binary_sensor.h"
#include "esphome/binary_sensor/custom_binary_sensor.h"
#include "esphome/binary_sensor/esp32_touch_binary_sensor.h"
#include "esphome/binary_sensor/filter.h"
#include "esphome/binary_sensor/gpio_binary_sensor_component.h"
#include "esphome/binary_sensor/homeassistant_binary_sensor.h"
#include "esphome/binary_sensor/mqtt_binary_sensor_component.h"
#include "esphome/binary_sensor/pn532_component.h"
#include "esphome/binary_sensor/rdm6300.h"
#include "esphome/binary_sensor/status_binary_sensor.h"
#include "esphome/binary_sensor/template_binary_sensor.h"
#include "esphome/binary_sensor/mpr121_sensor.h"
#include "esphome/binary_sensor/ttp229_lsf_sensor.h"
#include "esphome/climate/bang_bang_climate.h"
#include "esphome/climate/climate_device.h"
#include "esphome/climate/mqtt_climate_component.h"
#include "esphome/cover/cover.h"
#include "esphome/cover/endstop_cover.h"
#include "esphome/cover/time_based_cover.h"
#include "esphome/cover/mqtt_cover_component.h"
#include "esphome/cover/template_cover.h"
#include "esphome/display/display.h"
#include "esphome/display/lcd_display.h"
#include "esphome/display/max7219.h"
#include "esphome/display/nextion.h"
#include "esphome/display/ssd1306.h"
#include "esphome/display/waveshare_epaper.h"
#include "esphome/fan/basic_fan_component.h"
#include "esphome/fan/fan_state.h"
#include "esphome/fan/mqtt_fan_component.h"
#include "esphome/io/mcp23017.h"
#include "esphome/io/pcf8574_component.h"
#include "esphome/light/addressable_light_effect.h"
#include "esphome/light/fast_led_light_output.h"
#include "esphome/light/light_color_values.h"
#include "esphome/light/light_effect.h"
#include "esphome/light/light_output_component.h"
#include "esphome/light/light_state.h"
#include "esphome/light/mqtt_json_light_component.h"
#include "esphome/light/neo_pixel_bus_light_output.h"
#include "esphome/mqtt/custom_mqtt_device.h"
#include "esphome/mqtt/mqtt_client_component.h"
#include "esphome/mqtt/mqtt_component.h"
#include "esphome/output/binary_output.h"
#include "esphome/output/copy_output.h"
#include "esphome/output/custom_output.h"
#include "esphome/output/esp8266_pwm_output.h"
#include "esphome/output/float_output.h"
#include "esphome/output/gpio_binary_output_component.h"
#include "esphome/output/ledc_output_component.h"
#include "esphome/output/pca9685_output_component.h"
#include "esphome/output/my9231_output_component.h"
#include "esphome/output/sm16716_output_component.h"
#include "esphome/remote/jvc.h"
#include "esphome/remote/lg.h"
#include "esphome/remote/nec.h"
#include "esphome/remote/panasonic.h"
#include "esphome/remote/raw.h"
#include "esphome/remote/rc5.h"
#include "esphome/remote/rc_switch.h"
#include "esphome/remote/remote_receiver.h"
#include "esphome/remote/remote_transmitter.h"
#include "esphome/remote/samsung.h"
#include "esphome/remote/sony.h"
#include "esphome/sensor/adc.h"
#include "esphome/sensor/ads1115_component.h"
#include "esphome/sensor/apds9960.h"
#include "esphome/sensor/bh1750_sensor.h"
#include "esphome/sensor/bme280_component.h"
#include "esphome/sensor/bme680_component.h"
#include "esphome/sensor/bmp085_component.h"
#include "esphome/sensor/bmp280_component.h"
#include "esphome/sensor/cse7766.h"
#include "esphome/sensor/custom_sensor.h"
#include "esphome/sensor/dallas_component.h"
#include "esphome/sensor/dht12_component.h"
#include "esphome/sensor/dht_component.h"
#include "esphome/sensor/duty_cycle_sensor.h"
#include "esphome/sensor/esp32_hall_sensor.h"
#include "esphome/sensor/filter.h"
#include "esphome/sensor/hdc1080_component.h"
#include "esphome/sensor/hlw8012.h"
#include "esphome/sensor/hmc5883l.h"
#include "esphome/sensor/homeassistant_sensor.h"
#include "esphome/sensor/htu21d_component.h"
#include "esphome/sensor/hx711.h"
#include "esphome/sensor/ina219.h"
#include "esphome/sensor/ina3221.h"
#include "esphome/sensor/max31855_sensor.h"
#include "esphome/sensor/max6675_sensor.h"
#include "esphome/sensor/mhz19_component.h"
#include "esphome/sensor/mpu6050_component.h"
#include "esphome/sensor/mqtt_sensor_component.h"
#include "esphome/sensor/mqtt_subscribe_sensor.h"
#include "esphome/sensor/ms5611.h"
#include "esphome/sensor/pmsx003.h"
#include "esphome/sensor/pulse_counter.h"
#include "esphome/sensor/rotary_encoder.h"
#include "esphome/sensor/sensor.h"
#include "esphome/sensor/sht3xd_component.h"
#include "esphome/sensor/tcs34725.h"
#include "esphome/sensor/template_sensor.h"
#include "esphome/sensor/total_daily_energy.h"
#include "esphome/sensor/tsl2561_sensor.h"
#include "esphome/sensor/ultrasonic_sensor.h"
#include "esphome/sensor/uptime_sensor.h"
#include "esphome/sensor/wifi_signal_sensor.h"
#include "esphome/sensor/sds011_component.h"
#include "esphome/stepper/a4988.h"
#include "esphome/stepper/stepper.h"
#include "esphome/stepper/uln2003.h"
#include "esphome/switch_/custom_switch.h"
#include "esphome/switch_/gpio_switch.h"
#include "esphome/switch_/mqtt_switch_component.h"
#include "esphome/switch_/output_switch.h"
#include "esphome/switch_/restart_switch.h"
#include "esphome/switch_/shutdown_switch.h"
#include "esphome/switch_/switch.h"
#include "esphome/switch_/template_switch.h"
#include "esphome/switch_/uart_switch.h"
#include "esphome/text_sensor/custom_text_sensor.h"
#include "esphome/text_sensor/homeassistant_text_sensor.h"
#include "esphome/text_sensor/mqtt_subscribe_text_sensor.h"
#include "esphome/text_sensor/mqtt_text_sensor.h"
#include "esphome/text_sensor/template_text_sensor.h"
#include "esphome/text_sensor/text_sensor.h"
#include "esphome/text_sensor/version_text_sensor.h"
#include "esphome/text_sensor/wifi_info.h"
#include "esphome/time/rtc_component.h"
#include "esphome/time/sntp_component.h"
#include "esphome/time/homeassistant_time.h"

ESPHOME_NAMESPACE_BEGIN

/// This is the class that combines all components.
class Application {
 public:
  /** Set the name of the item that is running this app.
   *
   * Note: This will automatically be converted to lowercase_underscore.
   *
   * @param name The name of your app.
   */
  void set_name(const std::string &name);

  /** Initialize the logging engine.
   *
   * @param baud_rate The serial baud rate. Set to 0 to disable UART debugging.
   * @param tx_buffer_size The size of the printf buffer.
   * @param hw_serial The hardware serial UART used for logging.
   * @return The created and initialized LogComponent.
   */
  LogComponent *init_log(uint32_t baud_rate = 115200, size_t tx_buffer_size = 512,
                         UARTSelection uart = UART_SELECTION_UART0);

  /** Initialize the WiFi engine in client mode.
   *
   * Note: for advanced options, such as manual ip, use the return value.
   *
   * @param ssid The ssid of the network you want to connect to.
   * @param password The password of your network. Leave empty for no password
   * @return The WiFiComponent.
   */
  WiFiComponent *init_wifi(const std::string &ssid, const std::string &password = "");

  /// Initialize the WiFi engine with no initial mode. Use this if you just want an Access Point.
  WiFiComponent *init_wifi();

#ifdef USE_ETHERNET
  EthernetComponent *init_ethernet();
#endif

#ifdef USE_OTA
  /** Initialize Over-the-Air updates.
   *
   * @return The OTAComponent. Use this to set advanced settings.
   */
  OTAComponent *init_ota();
#endif

#ifdef USE_MQTT
  /** Initialize the MQTT client.
   *
   * @param address The address of your server.
   * @param port The port of your server.
   * @param username The username.
   * @param password The password. Empty for no password.
   * @return The MQTTClient. Use this to set advanced settings.
   */
  mqtt::MQTTClientComponent *init_mqtt(const std::string &address, uint16_t port, const std::string &username,
                                       const std::string &password);

  /** Initialize the MQTT client.
   *
   * @param address The address of your server.
   * @param username The username.
   * @param password The password. Empty for no password.
   * @return The MQTTClient. Use this to set advanced settings.
   */
  mqtt::MQTTClientComponent *init_mqtt(const std::string &address, const std::string &username,
                                       const std::string &password);
#endif

#ifdef USE_I2C
  /** Initialize the i2c bus on the provided SDA and SCL pins for use with other components.
   *
   * SDA/SCL pins default to the values defined by the Arduino framework and are usually
   * GPIO4 and GPIO5 on the ESP8266 (D2 and D1 on NodeMCU). And for the ESP32 it defaults to
   * GPIO21 and GPIO22 for SDA and SCL, respectively.
   *
   * @param sda_pin The SDA pin the i2c bus is connected to.
   * @param scl_pin The SCL pin the i2c bus is connected to.
   * @param scan If a scan of connected i2c devices should be done at startup.
   */
  I2CComponent *init_i2c(uint8_t sda_pin = SDA, uint8_t scl_pin = SCL, bool scan = false);
#endif

#ifdef USE_UART
  UARTComponent *init_uart(uint32_t baud_rate = 9600);
#endif

#ifdef USE_SPI
  SPIComponent *init_spi(const GPIOOutputPin &clk, const GPIOInputPin &miso, const GPIOOutputPin &mosi);

  SPIComponent *init_spi(const GPIOOutputPin &clk);
#endif

#ifdef USE_WEB_SERVER
  /** Initialize the web server. Note that this will take up quite a bit of flash space and
   * RAM of the node. Especially on ESP8266 boards this can quickly cause memory problems.
   *
   * @param port The port of the web server, defaults to 80.
   * @return The WebServer object, use this for advanced settings.
   */
  WebServer *init_web_server(uint16_t port = 80);
#endif

#ifdef USE_API
  api::APIServer *init_api_server();
#endif

#ifdef USE_ESP32_BLE_TRACKER
  /** Setup an ESP32 BLE Tracker Hub.
   *
   * Only one of these should be created. Individual sensors can be created using make_device() on
   * the return value.
   *
   * Note that this component is currently not completely stable (probably due to some issues in the
   * esp-idf core that are being worked on). Enabling this integration will increase the created binary
   * size by up to 500kB, can cause seemingly random reboots/resets and only can discover a small
   * number of devices at the moment.
   *
   * @return An ESP32BLETracker instance, use this to create individual trackers as binary sensors.
   */
  ESP32BLETracker *make_esp32_ble_tracker();
#endif

#ifdef USE_ESP32_BLE_BEACON
  ESP32BLEBeacon *make_esp32_ble_beacon(const std::array<uint8_t, 16> &uuid);
#endif

#ifdef USE_STATUS_LED
  StatusLEDComponent *make_status_led(const GPIOOutputPin &pin);
#endif

#ifdef USE_DISPLAY
  display::Font *make_font(std::vector<display::Glyph> &&glyphs, int baseline, int bottom);

  display::Image *make_image(const uint8_t *data_start, int width, int height);
#endif

#ifdef USE_MAX7219
  display::MAX7219Component *make_max7219(SPIComponent *parent, const GPIOOutputPin &cs,
                                          uint32_t update_interval = 1000);
#endif

#ifdef USE_LCD_DISPLAY_PCF8574
  display::PCF8574LCDDisplay *make_pcf8574_lcd_display(uint8_t columns, uint8_t rows, uint8_t address = 0x3F,
                                                       uint32_t update_interval = 1000);
#endif

#ifdef USE_LCD_DISPLAY
  display::GPIOLCDDisplay *make_gpio_lcd_display(uint8_t columns, uint8_t rows, uint32_t update_interval = 1000);
#endif

#ifdef USE_SSD1306
#ifdef USE_SPI
  display::SPISSD1306 *make_spi_ssd1306(SPIComponent *parent, const GPIOOutputPin &cs, const GPIOOutputPin &dc,
                                        uint32_t update_interval = 5000);
#endif
#ifdef USE_I2C
  display::I2CSSD1306 *make_i2c_ssd1306(uint32_t update_interval = 5000);
#endif
#endif

#ifdef USE_WAVESHARE_EPAPER
  display::WaveshareEPaperTypeA *make_waveshare_epaper_type_a(SPIComponent *parent, const GPIOOutputPin &cs,
                                                              const GPIOOutputPin &dc_pin,
                                                              display::WaveshareEPaperTypeAModel model,
                                                              uint32_t update_interval = 10000);

  display::WaveshareEPaper *make_waveshare_epaper_type_b(SPIComponent *parent, const GPIOOutputPin &cs,
                                                         const GPIOOutputPin &dc_pin,
                                                         display::WaveshareEPaperTypeBModel model,
                                                         uint32_t update_interval = 10000);
#endif

#ifdef USE_NEXTION
  display::Nextion *make_nextion(UARTComponent *parent, uint32_t update_interval = 5000);
#endif

  template<typename T> GlobalVariableComponent<T> *make_global_variable();

  template<typename T> GlobalVariableComponent<T> *make_global_variable(T initial_value);
  template<typename T>
  GlobalVariableComponent<T> *make_global_variable(
      std::array<typename std::remove_extent<T>::type, std::extent<T>::value> initial_value);

  /*           _    _ _______ ____  __  __       _______ _____ ____  _   _
   *      /\  | |  | |__   __/ __ \|  \/  |   /\|__   __|_   _/ __ \| \ | |
   *     /  \ | |  | |  | | | |  | | \  / |  /  \  | |    | || |  | |  \| |
   *    / /\ \| |  | |  | | | |  | | |\/| | / /\ \ | |    | || |  | | . ` |
   *   / ____ \ |__| |  | | | |__| | |  | |/ ____ \| |   _| || |__| | |\  |
   *  /_/    \_\____/   |_|  \____/|_|  |_/_/    \_\_|  |_____\____/|_| \_|
   */

  template<typename... Ts> Automation<Ts...> *make_automation(Trigger<Ts...> *trigger);

  StartupTrigger *make_startup_trigger();

  ShutdownTrigger *make_shutdown_trigger();

  /*   ____ ___ _   _    _    ______   __  ____  _____ _   _ ____   ___  ____
   *  | __ |_ _| \ | |  / \  |  _ \ \ / / / ___|| ____| \ | / ___| / _ \|  _ \
   *  |  _ \| ||  \| | / _ \ | |_) \ V /  \___ \|  _| |  \| \___ \| | | | |_) |
   *  | |_) | || |\  |/ ___ \|  _ < | |    ___) | |___| |\  |___) | |_| |  _ <
   *  |____|___|_| \_/_/   \_|_| \_\|_|   |____/|_____|_| \_|____/ \___/|_| \_\
   */
#ifdef USE_BINARY_SENSOR
  /// Register a binary sensor and set it up for the front-end.
  void register_binary_sensor(binary_sensor::BinarySensor *binary_sensor);
#endif

#ifdef USE_GPIO_BINARY_SENSOR
  /** Create a simple GPIO binary sensor.
   *
   * Note: advanced options such as inverted input are available in the return value.
   *
   * @param friendly_name The friendly name that should be advertised. Leave empty for no automatic discovery.
   * @param pin The GPIO pin.
   * @param device_class The Home Assistant <a
   * href="https://www.home-assistant.io/components/binary_sensor/">device_class</a>.
   */
  binary_sensor::GPIOBinarySensorComponent *make_gpio_binary_sensor(const std::string &friendly_name,
                                                                    const GPIOInputPin &pin,
                                                                    const std::string &device_class = "");
#endif

#ifdef USE_STATUS_BINARY_SENSOR
  /** Create a simple binary sensor that reports the online/offline state of the node.
   *
   * Uses the MQTT last will and birth message feature. If the values for these features are custom, you need
   * to override them using the return value of this function.
   *
   * @param friendly_name The friendly name advertised via MQTT discovery.
   */
  binary_sensor::StatusBinarySensor *make_status_binary_sensor(const std::string &friendly_name);
#endif

#ifdef USE_ESP32_TOUCH_BINARY_SENSOR
  /** Setup an ESP32TouchComponent to detect touches on certain pins of the ESP32 using the built-in touch peripheral.
   *
   * First set up the global hub using this method. Then create individual binary sensors using the make_touch_pad
   * function on the return type.
   *
   * @return The new ESP32TouchComponent. Use this to create the binary sensors.
   */
  binary_sensor::ESP32TouchComponent *make_esp32_touch_component();
#endif

#ifdef USE_TEMPLATE_BINARY_SENSOR
  binary_sensor::TemplateBinarySensor *make_template_binary_sensor(const std::string &name);
#endif

#ifdef USE_REMOTE_RECEIVER
  remote::RemoteReceiverComponent *make_remote_receiver_component(const GPIOInputPin &output);
#endif

#ifdef USE_PN532
  binary_sensor::PN532Component *make_pn532_component(SPIComponent *parent, const GPIOOutputPin &cs,
                                                      uint32_t update_interval = 1000);
#endif

#ifdef USE_RDM6300
  binary_sensor::RDM6300Component *make_rdm6300_component(UARTComponent *parent);
#endif

#ifdef USE_CUSTOM_BINARY_SENSOR
  binary_sensor::CustomBinarySensorConstructor *make_custom_binary_sensor(
      const std::function<std::vector<binary_sensor::BinarySensor *>()> &init);
#endif

#ifdef USE_MPR121
  binary_sensor::MPR121Component *make_mpr121(uint8_t address = 0x5A);
#endif

#ifdef USE_TTP229_LSF
  binary_sensor::TTP229LSFComponent *make_ttp229_lsf(uint8_t address = 0x57);
#endif

  /*   ____  _____ _   _ ____   ___  ____
   *  / ___|| ____| \ | / ___| / _ \|  _ \
   *  \___ \|  _| |  \| \___ \| | | | |_) |
   *   ___) | |___| |\  |___) | |_| |  _ <
   *  |____/|_____|_| \_|____/ \___/|_| \_\
   */
#ifdef USE_SENSOR
  /// Register a sensor and create a MQTT Sensor if the MQTT client is set up
  void register_sensor(sensor::Sensor *sensor);
#endif

#ifdef USE_TEXT_SENSOR
  /// Register a text sensor and create a MQTT Sensor if the MQTT client is set up
  void register_text_sensor(text_sensor::TextSensor *sensor);
#endif

#ifdef USE_DHT_SENSOR
  /** Create a DHT sensor component.
   *
   * Note: This method automatically applies a SlidingWindowMovingAverageFilter.
   *
   * @param temperature_friendly_name The name the temperature sensor should be advertised as. Leave empty for no
   *                                  automatic discovery.
   * @param humidity_friendly_name The name the humidity sensor should be advertised as. Leave empty for no
   *                                  automatic discovery.
   * @param pin The pin the DHT sensor is connected to.
   * @param update_interval The interval (in ms) the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  sensor::DHTComponent *make_dht_sensor(const std::string &temperature_friendly_name,
                                        const std::string &humidity_friendly_name, const GPIOOutputPin &pin,
                                        uint32_t check_interval = 60000);
#endif

#ifdef USE_DALLAS_SENSOR
  sensor::DallasComponent *make_dallas_component(const GPIOOutputPin &pin, uint32_t update_interval = 60000);
#endif

#ifdef USE_PULSE_COUNTER_SENSOR
  /** Create an ESP32 Pulse Counter component.
   *
   * The pulse counter peripheral will automatically all pulses on pin in the background. Every
   * check_interval ms the amount of pulses will be retrieved and the difference to the last value
   * will be reported via MQTT as a sensor.
   *
   * @param pin The pin the pulse counter should count pulses on.
   * @param friendly_name The name the sensor should be advertised as.
   * @param update_interval The interval in ms the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  sensor::PulseCounterSensorComponent *make_pulse_counter_sensor(const std::string &friendly_name,
                                                                 const GPIOInputPin &pin,
                                                                 uint32_t update_interval = 60000);
#endif

#ifdef USE_ADC_SENSOR
  /** Create an ADC Sensor component.
   *
   * Every check_interval ms, the value from the specified pin (only A0 on ESP8266, 32-39 for ESP32),
   * and converts it into the volt unit. On the ESP32 you can additionally specify a channel attenuation
   * using the return value of this function. pinMode can also be set using the return value.
   *
   * @param pin The pin the ADC should sense on.
   * @param friendly_name The name the sensor should be advertised as.
   * @param update_interval The interval in ms the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  sensor::ADCSensorComponent *make_adc_sensor(const std::string &friendly_name, uint8_t pin,
                                              uint32_t update_interval = 60000);
#endif

#ifdef USE_ADS1115_SENSOR
  /** Create an ADS1115 component hub. From this hub you can then create individual sensors using `get_sensor()`.
   *
   * Note that you should have i2c setup for this component to work. To setup i2c call `App.init_i2c(SDA_PIN, SCL_PIN);`
   * before `App.setup()`.
   *
   * @param address The i2c address of the ADS1115. See ADS1115Component::set_address for possible values.
   * @return The ADS1115Component hub. Use this to set advanced setting and create the actual sensors.
   */
  sensor::ADS1115Component *make_ads1115_component(uint8_t address);
#endif

#ifdef USE_BMP085_SENSOR
  /** Create an BMP085/BMP180/BMP280 i2c temperature+pressure sensor.
   *
   * Be sure to initialize i2c before calling `App.setup()` in order for this to work. Do so
   * with `App.init_i2c(SDA_PIN, SCL_PIN);`.
   *
   * @param temperature_friendly_name The friendly name the temperature should be advertised as.
   * @param pressure_friendly_name The friendly name the pressure should be advertised as.
   * @param update_interval The interval in ms to update the sensor values.
   */
  sensor::BMP085Component *make_bmp085_sensor(const std::string &temperature_friendly_name,
                                              const std::string &pressure_friendly_name,
                                              uint32_t update_interval = 60000);
#endif

#ifdef USE_HTU21D_SENSOR

  /** Create a HTU21D i2c-based temperature+humidity highly accurate sensor.
   *
   * Be sure to initialize i2c before calling `App.setup` in order for this to work. Do so
   * with `App.init_i2c(SDA_PIN, SCL_PIN);`.
   *
   * @param temperature_friendly_name The friendly name the temperature sensor should be advertised as.
   * @param humidity_friendly_name The friendly name the humidity sensor should be advertised as.
   * @param update_interval The interval in ms to update the sensor values.
   */
  sensor::HTU21DComponent *make_htu21d_sensor(const std::string &temperature_friendly_name,
                                              const std::string &humidity_friendly_name,
                                              uint32_t update_interval = 60000);
#endif

#ifdef USE_HDC1080_SENSOR
  /** Create a HDC1080 i2c-based temperature+humidity sensor.
   *
   * Be sure to initialize i2c before calling `App.setup` in order for this to work. Do so
   * with `App.init_i2c(SDA_PIN, SCL_PIN);`.
   *
   * @param temperature_friendly_name The friendly name the temperature sensor should be advertised as.
   * @param humidity_friendly_name The friendly name the humidity sensor should be advertised as.
   * @param update_interval The interval in ms to update the sensor values.
   */
  sensor::HDC1080Component *make_hdc1080_sensor(const std::string &temperature_friendly_name,
                                                const std::string &humidity_friendly_name,
                                                uint32_t update_interval = 60000);
#endif

#ifdef USE_ULTRASONIC_SENSOR
  /** Create an Ultrasonic range sensor.
   *
   * This can for example be an HC-SR04 ultrasonic sensor. It sends out a short ultrasonic wave and listens
   * for an echo. The time between the sending and receiving is then (with some maths) converted to a measurement
   * in meters. You need to specify the trigger pin (where to short pulse will be sent to) and the echo pin
   * (where we're waiting for the echo). Note that in order to not block indefinitely if we don't receive an
   * echo, this class has a default timeout of around 2m. You can change that using the return value of this
   * function.
   *
   * @param friendly_name The friendly name for this sensor advertised to Home Assistant.
   * @param trigger_pin The pin the short pulse will be sent to, can be integer or GPIOOutputPin.
   * @param echo_pin The pin we wait that we wait on for the echo, can be integer or GPIOInputPin.
   * @param update_interval The time in ms between updates, defaults to 60 seconds.
   */
  sensor::UltrasonicSensorComponent *make_ultrasonic_sensor(const std::string &friendly_name,
                                                            const GPIOOutputPin &trigger_pin,
                                                            const GPIOInputPin &echo_pin,
                                                            uint32_t update_interval = 60000);
#endif

#ifdef USE_WIFI_SIGNAL_SENSOR
  sensor::WiFiSignalSensor *make_wifi_signal_sensor(const std::string &name, uint32_t update_interval = 60000);
#endif

#ifdef USE_MPU6050
  /** Create a MPU6050 Accelerometer+Gyroscope+Temperature sensor hub.
   *
   * This integration can be used to get accurate accelerometer readings and uncalibrated gyroscope
   * values (in degrees per second). If you need the latter with calibration applied, your best bet
   * it to just copy the source and do it yourself, as calibration must be performed while the sensor
   * is at rest and this property can not be asserted for all use cases.
   *
   * @param address The address of the device, defaults to 0x68.
   * @param update_interval The interval in ms to update the sensor values.
   * @return An MPU6050Component, use this to create the individual sensors and register them with `register_sensor`.
   */
  sensor::MPU6050Component *make_mpu6050_sensor(uint8_t address = 0x68, uint32_t update_interval = 60000);
#endif

#ifdef USE_TSL2561
  /** Create a TSL2561 accurate ambient light sensor.
   *
   * This i2c-based device can provide very precise illuminance readings with great accuracy regarding the human
   * eye response to the brightness level. By default, this sensor uses the i2c address 0x39, but you can change it
   * with `set_address` later using the return value of this function (address 0x29 if '0' shorted on board, address
   * 0x49 if '1' shorted on board).
   *
   * The sensor values that are pushed out will be the transformed illuminance values in lx taking using the
   * internal IR and Full Spectrum photodiodes.
   *
   * Additionally, you can specify the time the sensor takes for accumulating the values (higher is better for
   * lower light conditions, defaults to 402ms - the max) and a gain that should be used for the ADCs (defaults to
   * 1x). Finally, this integration is energy efficient and only turns on the sensor when the values are read out.
   *
   * @param name The friendly name how the sensor should be advertised.
   * @param address The address of this i2c device.
   * @param update_interval The interval in ms to update the sensor values.
   */
  sensor::TSL2561Sensor *make_tsl2561_sensor(const std::string &name, uint8_t address = 0x23,
                                             uint32_t update_interval = 60000);
#endif

#ifdef USE_BH1750
  /** Create a BH1750 ambient light sensor.
   *
   * This i2c-based provides ambient light readings in lx with resolutions of 4LX, 1LX and 0.5LX (the default).
   * To change the resolution, call set_resolution on the return value of this function.
   *
   * By default, this sensor uses the i2c address 0x23 (the default if ADDR is pulled low). If the ADDR pin
   * is pulled high (above 0.7VCC), then you can manually set the address to 0x5C using set_address.
   *
   * @param name The friendly name that this sensor should be advertised as.
   * @param address The address of this i2c device.
   * @param update_interval The interval in ms to update the sensor values.
   * @return The BH1750Sensor + MQTT sensor pair, use this for advanced settings.
   */
  sensor::BH1750Sensor *make_bh1750_sensor(const std::string &name, uint8_t address = 0x23,
                                           uint32_t update_interval = 60000);
#endif

#ifdef USE_BME280
  /** Create a BME280 Temperature+Pressure+Humidity i2c sensor.
   *
   * @param temperature_name The friendly name the temperature sensor should be advertised as.
   * @param pressure_name The friendly name the pressure sensor should be advertised as.
   * @param humidity_name The friendly name the humidity sensor should be advertised as.
   * @param address The i2c address of the sensor. Defaults to 0x77 (SDO to V_DDIO), can also be 0x76.
   * @param update_interval The interval in ms to update the sensor values.
   */
  sensor::BME280Component *make_bme280_sensor(const std::string &temperature_name, const std::string &pressure_name,
                                              const std::string &humidity_name, uint8_t address = 0x77,
                                              uint32_t update_interval = 60000);
#endif

#ifdef USE_BMP280
  /** Create a BMP280 Temperature+Pressure i2c sensor.
   *
   * @param temperature_name The friendly name the temperature sensor should be advertised as.
   * @param pressure_name The friendly name the pressure sensor should be advertised as.
   * @param address The i2c address of the sensor. Defaults to 0x77 (SDO to V_DDIO), can also be 0x76.
   * @param update_interval The interval in ms to update the sensor values.
   */
  sensor::BMP280Component *make_bmp280_sensor(const std::string &temperature_name, const std::string &pressure_name,
                                              uint8_t address = 0x77, uint32_t update_interval = 60000);
#endif

#ifdef USE_BME680
  sensor::BME680Component *make_bme680_sensor(const std::string &temperature_name, const std::string &pressure_name,
                                              const std::string &humidity_name, const std::string &gas_resistance_name,
                                              uint8_t address = 0x76, uint32_t update_interval = 60000);
#endif

#ifdef USE_SHT3XD
  sensor::SHT3XDComponent *make_sht3xd_sensor(const std::string &temperature_name, const std::string &humidity_name,
                                              uint8_t address = 0x44, uint32_t update_interval = 60000);
#endif

#ifdef USE_DHT12_SENSOR
  sensor::DHT12Component *make_dht12_sensor(const std::string &temperature_name, const std::string &humidity_name,
                                            uint32_t update_interval = 60000);
#endif

#ifdef USE_ROTARY_ENCODER_SENSOR
  /** Create a continuous rotary encoder sensor with a digital signal.
   *
   * It will keep track of how far the encoder has been turned using the signals from the two required pins A & B.
   * There's also support for a third "index" pin. Each time this pin is pulled high, the counter will reset to 0.
   *
   * Additionally, you can specify a resolution for the rotary encoder. By default, the encoder will only increment
   * the counter once a full cycle of A&B signals has been detected to prevent triggers from noise. You can change
   * this behavior using the set_resolution method.
   *
   * The output value of this rotary encoder is a raw integer step value. Use filters
   * to convert this raw value to something sensible like degrees. Next, this sensor pushes its state on every detected
   * counter change.
   *
   * Read https://playground.arduino.cc/Main/RotaryEncoders to see how they work.
   *
   * @param name The name of the rotary encoder.
   * @param pin_a The first pin of the sensor.
   * @param pin_b The second pin of the sensor.
   */
  sensor::RotaryEncoderSensor *make_rotary_encoder_sensor(const std::string &name, const GPIOInputPin &pin_a,
                                                          const GPIOInputPin &pin_b);
#endif

#ifdef USE_TEMPLATE_SENSOR
  sensor::TemplateSensor *make_template_sensor(const std::string &name, uint32_t update_interval = 60000);
#endif

#ifdef USE_MAX31855_SENSOR
  sensor::MAX31855Sensor *make_max31855_sensor(const std::string &name, SPIComponent *spi_bus, const GPIOOutputPin &cs,
                                               uint32_t update_interval = 60000);
#endif

#ifdef USE_MAX6675_SENSOR
  sensor::MAX6675Sensor *make_max6675_sensor(const std::string &name, SPIComponent *spi_bus, const GPIOOutputPin &cs,
                                             uint32_t update_interval = 60000);
#endif

#ifdef USE_ESP32_HALL_SENSOR
  sensor::ESP32HallSensor *make_esp32_hall_sensor(const std::string &name, uint32_t update_interval = 60000);
#endif

#ifdef USE_DUTY_CYCLE_SENSOR
  sensor::DutyCycleSensor *make_duty_cycle_sensor(const std::string &name, const GPIOInputPin &pin,
                                                  uint32_t update_interval = 60000);
#endif

#ifdef USE_MHZ19
  sensor::MHZ19Component *make_mhz19_sensor(UARTComponent *parent, const std::string &co2_name,
                                            uint32_t update_interval = 60000);
#endif

#ifdef USE_UPTIME_SENSOR
  sensor::UptimeSensor *make_uptime_sensor(const std::string &name, uint32_t update_interval = 60000);
#endif

#ifdef USE_INA219
  sensor::INA219Component *make_ina219(float shunt_resistance_ohm, float max_current_a, float max_voltage_v,
                                       uint8_t address = 0x40, uint32_t update_interval = 60000);
#endif

#ifdef USE_INA3221
  sensor::INA3221Component *make_ina3221(uint8_t address = 0x40, uint32_t update_interval = 60000);
#endif

#ifdef USE_HMC5883L
  sensor::HMC5883LComponent *make_hmc5883l(uint32_t update_interval = 60000);
#endif

#ifdef USE_HX711
  sensor::HX711Sensor *make_hx711_sensor(const std::string &name, const GPIOInputPin &dout, const GPIOOutputPin &sck,
                                         uint32_t update_interval = 60000);
#endif

#ifdef USE_MS5611
  sensor::MS5611Component *make_ms5611_sensor(const std::string &temperature_name, const std::string &pressure_name,
                                              uint32_t update_interval = 60000);
#endif

#ifdef USE_TCS34725
  sensor::TCS34725Component *make_tcs34725(uint32_t update_interval = 60000);
#endif

#ifdef USE_SNTP_COMPONENT
  time::SNTPComponent *make_sntp_component();
#endif

#ifdef USE_HOMEASSISTANT_TIME
  time::HomeAssistantTime *make_homeassistant_time_component();
#endif

#ifdef USE_HLW8012
  sensor::HLW8012Component *make_hlw8012(const GPIOOutputPin &sel_pin, uint8_t cf_pin, uint8_t cf1_pin,
                                         uint32_t update_interval = 60000);
#endif

#ifdef USE_MQTT_SUBSCRIBE_SENSOR
  sensor::MQTTSubscribeSensor *make_mqtt_subscribe_sensor(const std::string &name, std::string topic);
#endif

#ifdef USE_HOMEASSISTANT_SENSOR
  sensor::HomeassistantSensor *make_homeassistant_sensor(const std::string &name, std::string entity_id);
#endif

#ifdef USE_HOMEASSISTANT_BINARY_SENSOR
  binary_sensor::HomeassistantBinarySensor *make_homeassistant_binary_sensor(const std::string &name,
                                                                             std::string entity_id);
#endif

#ifdef USE_CSE7766
  sensor::CSE7766Component *make_cse7766(UARTComponent *parent, uint32_t update_interval = 60000);
#endif

#ifdef USE_MQTT_SUBSCRIBE_TEXT_SENSOR
  text_sensor::MQTTSubscribeTextSensor *make_mqtt_subscribe_text_sensor(const std::string &name, std::string topic);
#endif

#ifdef USE_HOMEASSISTANT_TEXT_SENSOR
  text_sensor::HomeassistantTextSensor *make_homeassistant_text_sensor(const std::string &name, std::string entity_id);
#endif

#ifdef USE_VERSION_TEXT_SENSOR
  text_sensor::VersionTextSensor *make_version_text_sensor(const std::string &name);
#endif

#ifdef USE_TEMPLATE_TEXT_SENSOR
  text_sensor::TemplateTextSensor *make_template_text_sensor(const std::string &name, uint32_t update_interval = 60000);
#endif

#ifdef USE_PMSX003
  sensor::PMSX003Component *make_pmsx003(UARTComponent *parent, sensor::PMSX003Type type);
#endif

#ifdef USE_TOTAL_DAILY_ENERGY_SENSOR
  sensor::TotalDailyEnergy *make_total_daily_energy_sensor(const std::string &name, time::RealTimeClockComponent *time,
                                                           sensor::Sensor *parent);
#endif

#ifdef USE_APDS9960
  sensor::APDS9960 *make_apds9960(uint32_t update_interval = 60000);
#endif

#ifdef USE_CUSTOM_SENSOR
  sensor::CustomSensorConstructor *make_custom_sensor(const std::function<std::vector<sensor::Sensor *>()> &init);
#endif

#ifdef USE_SDS011
  sensor::SDS011Component *make_sds011(UARTComponent *parent);
#endif

  /*    ___  _   _ _____ ____  _   _ _____
   *   / _ \| | | |_   _|  _ \| | | |_   _|
   *  | | | | | | | | | | |_) | | | | | |
   *  | |_| | |_| | | | |  __/| |_| | | |
   *   \___/ \___/  |_| |_|    \___/  |_|
   */
#ifdef USE_OUTPUT
  /** Create a power supply component that will automatically switch on and off.
   *
   * @param pin The pin the power supply is connected to.
   * @param enable_time The time (in ms) the power supply needs until it can provide high power when powering on.
   * @param keep_on_time The time (in ms) the power supply should stay on when it is not used.
   * @return The PowerSupplyComponent.
   */
  PowerSupplyComponent *make_power_supply(const GPIOOutputPin &pin, uint32_t enable_time = 20,
                                          uint32_t keep_on_time = 10000);
#endif

#ifdef USE_LEDC_OUTPUT
  /** Create a ESP32 LEDC channel.
   *
   * @param pin The pin.
   * @param frequency The PWM frequency.
   * @param bit_depth The LEDC bit depth.
   * @return The LEDC component. Use this for advanced settings.
   */
  output::LEDCOutputComponent *make_ledc_output(uint8_t pin, float frequency = 1000.0f, uint8_t bit_depth = 12);
#endif

#ifdef USE_PCA9685_OUTPUT
  /** Create a PCA9685 component.
   *
   * @param frequency The PWM frequency.
   * @return The PCA9685 component. Use this for advanced settings.
   */
  output::PCA9685OutputComponent *make_pca9685_component(float frequency);
#endif

#ifdef USE_GPIO_OUTPUT
  /** Create a simple binary GPIO output component.
   *
   * Note: This is *only* a binary output component, not a switch that will be exposed
   * in Home Assistant. See make_simple_gpio_switch for a switch.
   *
   * @param pin The GPIO pin.
   * @return The GPIOBinaryOutputComponent. Use this for advanced settings.
   */
  output::GPIOBinaryOutputComponent *make_gpio_output(const GPIOOutputPin &pin);
#endif

#ifdef USE_ESP8266_PWM_OUTPUT
  /** Create an ESP8266 software PWM channel.
   *
   * Warning: This is a *software* PWM and therefore can have noticeable flickering. Additionally,
   * this software PWM can't output values higher than 80%.
   *
   * @param pin The pin for this PWM output, supported pins are 0-16.
   * @return The PWM output channel, use this for advanced settings and using it with lights.
   */
  output::ESP8266PWMOutput *make_esp8266_pwm_output(GPIOOutputPin pin);
#endif

#ifdef USE_MY9231_OUTPUT
  /** Create a MY9231 component.
   *
   * @param pin_di The pin which DI is connected to.
   * @param pin_dcki The pin which DCKI is connected to.
   * @return The MY9231 component. Use this for advanced settings.
   */
  output::MY9231OutputComponent *make_my9231_component(const GPIOOutputPin &pin_di, const GPIOOutputPin &pin_dcki);
#endif

#ifdef USE_SM16716_OUTPUT
  /** Create a SM16716 component.
   *
   * @param pin_data The data output pin, connected to the SM16716's DIN pin.
   * @param pin_clock The clock pin, connected to the SM16716's DCLK pin.
   * @return The SM16716 component. Use this for advanced settings.
   */
  output::SM16716OutputComponent *make_sm16716_component(const GPIOOutputPin &pin_data, const GPIOOutputPin &pin_clock);
#endif

  /*   _     ___ ____ _   _ _____
   *  | |   |_ _/ ___| | | |_   _|
   *  | |    | | |  _| |_| | | |
   *  | |___ | | |_| |  _  | | |
   *  |_____|___\____|_| |_| |_|
   */
#ifdef USE_LIGHT
  /// Register a light.
  void register_light(light::LightState *state);

  struct MakeLight {
    light::LightOutput *output;
    light::LightState *state;
  };

  MakeLight make_light_for_light_output(const std::string &name, light::LightOutput *output);

  /** Create a binary light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param binary The binary output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  MakeLight make_binary_light(const std::string &friendly_name, output::BinaryOutput *binary);

  /** Create a monochromatic light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param mono The output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  MakeLight make_monochromatic_light(const std::string &friendly_name, output::FloatOutput *mono);

  /** Create a RGB light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param red The red output channel.
   * @param green The green output channel.
   * @param blue The blue output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  MakeLight make_rgb_light(const std::string &friendly_name, output::FloatOutput *red, output::FloatOutput *green,
                           output::FloatOutput *blue);

  /** Create a RGBW light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param red The red output channel.
   * @param green The green output channel.
   * @param blue The blue output channel.
   * @param white The white output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  MakeLight make_rgbw_light(const std::string &friendly_name, output::FloatOutput *red, output::FloatOutput *green,
                            output::FloatOutput *blue, output::FloatOutput *white);

  MakeLight make_rgbww_light(const std::string &friendly_name, float cold_white_mireds, float warm_white_mireds,
                             output::FloatOutput *red, output::FloatOutput *green, output::FloatOutput *blue,
                             output::FloatOutput *cold_white, output::FloatOutput *warm_white);

  MakeLight make_cwww_light(const std::string &friendly_name, float cold_white_mireds, float warm_white_mireds,
                            output::FloatOutput *cold_white, output::FloatOutput *warm_white);
#endif

#ifdef USE_FAST_LED_LIGHT
  struct MakeFastLEDLight {
    light::FastLEDLightOutputComponent *fast_led;
    light::LightState *state;
  };

  /// Create an FastLED light.
  MakeFastLEDLight make_fast_led_light(const std::string &name);
#endif

#ifdef USE_NEO_PIXEL_BUS_LIGHT
  template<typename T_METHOD, typename T_COLOR_FEATURE> struct MakeNeoPixelBusLight {
    light::NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE> *output;
    light::LightState *state;
  };

  /// Create an RGB NeoPixelBus light.
  template<typename T_METHOD, typename T_COLOR_FEATURE = NeoRgbFeature>
  MakeNeoPixelBusLight<T_METHOD, T_COLOR_FEATURE> make_neo_pixel_bus_rgb_light(const std::string &name);

  /// Create an RGBW NeoPixelBus light.
  template<typename T_METHOD, typename T_COLOR_FEATURE = NeoRgbwFeature>
  MakeNeoPixelBusLight<T_METHOD, T_COLOR_FEATURE> make_neo_pixel_bus_rgbw_light(const std::string &name);
#endif

#ifdef USE_LIGHT
  struct MakePartitionLight {
    light::PartitionLightOutput *partition;
    light::LightState *state;
  };
  MakePartitionLight make_partition_light(const std::string &name,
                                          const std::vector<light::AddressableSegment> &segments);
#endif

  /*   ______        _____ _____ ____ _   _
   *  / ___\ \      / |_ _|_   _/ ___| | | |
   *  \___ \\ \ /\ / / | |  | || |   | |_| |
   *   ___) |\ V  V /  | |  | || |___|  _  |
   *  |____/  \_/\_/  |___| |_| \____|_| |_|
   */
#ifdef USE_SWITCH
  /// Register a Switch internally, creating a MQTT Switch if the MQTT client is set up
  void register_switch(switch_::Switch *a_switch);
#endif

#ifdef USE_GPIO_SWITCH
  /** Create a simple GPIO switch that can be toggled on/off and appears in the frontend.
   *
   * @param pin The pin used for this switch. Can be integer or GPIOOutputPin.
   * @param friendly_name The friendly name advertised to Home Assistant for this switch-
   */
  switch_::GPIOSwitch *make_gpio_switch(const std::string &friendly_name, const GPIOOutputPin &pin);
#endif

#ifdef USE_RESTART_SWITCH
  /// Make a simple switch that restarts the device with the provided friendly name.
  switch_::RestartSwitch *make_restart_switch(const std::string &friendly_name);
#endif

#ifdef USE_SHUTDOWN_SWITCH
  /// Make a simple switch that shuts the node down indefinitely.
  switch_::ShutdownSwitch *make_shutdown_switch(const std::string &friendly_name);
#endif

#ifdef USE_OUTPUT_SWITCH
  /// Make an output switch that exposes a binary output as a switch
  switch_::OutputSwitch *make_output_switch(const std::string &friendly_name, output::BinaryOutput *output);
#endif

#ifdef USE_TEMPLATE_SWITCH
  switch_::TemplateSwitch *make_template_switch(const std::string &name);
#endif

#ifdef USE_REMOTE_TRANSMITTER
  remote::RemoteTransmitterComponent *make_remote_transmitter_component(const GPIOOutputPin &output);
#endif

#ifdef USE_UART_SWITCH
  switch_::UARTSwitch *make_uart_switch(UARTComponent *parent, const std::string &name,
                                        const std::vector<uint8_t> &data);
#endif

#ifdef USE_CUSTOM_SWITCH
  switch_::CustomSwitchConstructor *make_custom_switch(const std::function<std::vector<switch_::Switch *>()> &init);
#endif

  /*   _____ _    _   _
   *  |  ___/ \  | \ | |
   *  | |_ / _ \ |  \| |
   *  |  _/ ___ \| |\  |
   *  |_|/_/   \_|_| \_|
   */
#ifdef USE_FAN
  /// Register a fan internally.
  void register_fan(fan::FanState *state);

  struct MakeFan {
    fan::BasicFanComponent *output;
    fan::FanState *state;
  };

  /** Create and connect a Fan with the specified friendly name.
   *
   * @param friendly_name The friendly name of the Fan to advertise.
   * @return A FanStruct, use the output field to set your output channels.
   */
  MakeFan make_fan(const std::string &friendly_name);
#endif

  /*    _____ ______      ________ _____
   *   / ____/ __ \ \    / /  ____|  __ \
   *  | |   | |  | \ \  / /| |__  | |__) |
   *  | |   | |  | |\ \/ / |  __| |  _  /
   *  | |___| |__| | \  /  | |____| | \ \
   *   \_____\____/   \/   |______|_|  \_\
   */
#ifdef USE_COVER
  void register_cover(cover::Cover *cover);
#endif

#ifdef USE_A4988
  stepper::A4988 *make_a4988(const GPIOOutputPin &step_pin, const GPIOOutputPin &dir_pin);
#endif

#ifdef USE_ULN2003
  stepper::ULN2003 *make_uln2003(const GPIOOutputPin &pin_a, const GPIOOutputPin &pin_b, const GPIOOutputPin &pin_c,
                                 const GPIOOutputPin &pin_d);
#endif

#ifdef USE_CLIMATE
  void register_climate(climate::ClimateDevice *climate);
#endif

  /*   _   _ _____ _     ____  _____ ____  ____
   *  | | | | ____| |   |  _ \| ____|  _ \/ ___|
   *  | |_| |  _| | |   | |_) |  _| | |_) \___ \
   *  |  _  | |___| |___|  __/| |___|  _ < ___) |
   *  |_| |_|_____|_____|_|   |_____|_| \_|____/
   */
#ifdef USE_DEBUG_COMPONENT
  DebugComponent *make_debug_component();
#endif

#ifdef USE_DEEP_SLEEP
  DeepSleepComponent *make_deep_sleep_component();
#endif

#ifdef USE_PCF8574
  /** Create a PCF8574/PCF8575 port expander component.
   *
   * This component will allow you to emulate GPIOInputPin and GPIOOutputPin instances that
   * are used within ESPHome. You can therefore simply pass the result of calling
   * `make_pin` on the component to any method accepting GPIOInputPin or GPIOOutputPin.
   *
   * Optionally, this component also has support for the 16-channel PCF8575 port expander.
   * To use the PCF8575, set the pcf8575 in this helper function.
   *
   * @param address The i2c address to use for this port expander. Defaults to 0x21.
   * @param pcf8575 If this is an PCF8575. Defaults to PCF8574.
   * @return The PCF8574Component instance to get individual pins.
   */
  io::PCF8574Component *make_pcf8574_component(uint8_t address = 0x21, bool pcf8575 = false);
#endif

#ifdef USE_MCP23017
  io::MCP23017 *make_mcp23017_component(uint8_t address = 0x20);
#endif

  /// Register the component in this Application instance.
  template<class C> C *register_component(C *c);

  template<class C> C *register_controller(C *c);

  /// Set up all the registered components. Call this at the end of your setup() function.
  void setup();

  /// Make a loop iteration. Call this in your loop() function.
  void loop();

  WiFiComponent *get_wifi() const;
#ifdef USE_MQTT
  mqtt::MQTTClientComponent *get_mqtt_client() const;
#endif

  /// Get the name of this Application set by set_name().
  const std::string &get_name() const;

  bool is_fully_setup() const;

  /** Tell ESPHome when your project was last compiled. This is used to show
   * a message like "You're running ESPHome v1.9.0 compiled on Oct 10 2018, 16:42:00"
   *
   * To use this method in code, put the following before App.setup():
   *
   * ```cpp
   * App.set_compilation_datetime(__DATE__ ", " __TIME__);
   * ```
   *
   * @param str The string of the time of compilation.
   */
  void set_compilation_datetime(const char *str);

  const std::string &get_compilation_time() const;

  /** Set the target interval with which to run the loop() calls.
   * If the loop() method takes longer than the target interval, ESPHome won't
   * sleep in loop(), but if the time spent in loop() is small than the target, ESPHome
   * will delay at the end of the App.loop() method.
   *
   * This is done to conserve power: In most use-cases, high-speed loop() calls are not required
   * and degrade power consumption.
   *
   * Each component can request a high frequency loop execution by using the HighFrequencyLoopRequester
   * helper in helpers.h
   *
   * @param loop_interval The interval in milliseconds to run the core loop at. Defaults to 16 milliseconds.
   */
  void set_loop_interval(uint32_t loop_interval);

  void dump_config();
  void schedule_dump_config();

 protected:
  void register_component_(Component *comp);

  std::vector<Component *> components_{};
  std::vector<Controller *> controllers_{};
#ifdef USE_MQTT
  mqtt::MQTTClientComponent *mqtt_client_{nullptr};
#endif
  WiFiComponent *wifi_{nullptr};

  std::string name_;
  std::string compilation_time_;
  uint32_t application_state_{COMPONENT_STATE_CONSTRUCTION};
  uint32_t last_loop_{0};
  uint32_t loop_interval_{16};
#ifdef USE_I2C
  I2CComponent *i2c_{nullptr};
#endif
  bool dump_config_scheduled_{false};
};

/// Global storage of Application pointer - only one Application can exist.
extern Application App;

template<class C> C *Application::register_component(C *c) {
  static_assert(std::is_base_of<Component, C>::value, "Only Component subclasses can be registered");
  this->register_component_((Component *) c);
  return c;
}

template<class C> C *Application::register_controller(C *c) {
  static_assert(std::is_base_of<Controller, C>::value, "Only Controller subclasses can be registered");
  Controller *controller = c;
  this->controllers_.push_back(controller);
  return c;
}

template<typename... Ts> Automation<Ts...> *Application::make_automation(Trigger<Ts...> *trigger) {
  return new Automation<Ts...>(trigger);
}

template<typename T> GlobalVariableComponent<T> *Application::make_global_variable() {
  return this->register_component(new GlobalVariableComponent<T>());
}

template<typename T> GlobalVariableComponent<T> *Application::make_global_variable(T initial_value) {
  return this->register_component(new GlobalVariableComponent<T>(initial_value));
}
template<typename T>
GlobalVariableComponent<T> *Application::make_global_variable(
    std::array<typename std::remove_extent<T>::type, std::extent<T>::value> initial_value) {
  return this->register_component(new GlobalVariableComponent<T>(initial_value));
}

#ifdef USE_NEO_PIXEL_BUS_LIGHT
template<typename T_METHOD, typename T_COLOR_FEATURE>
Application::MakeNeoPixelBusLight<T_METHOD, T_COLOR_FEATURE> Application::make_neo_pixel_bus_rgb_light(
    const std::string &name) {
  auto *neo_pixel = this->register_component(new light::NeoPixelRGBLightOutput<T_METHOD, T_COLOR_FEATURE>());
  auto make = this->make_light_for_light_output(name, neo_pixel);

  return MakeNeoPixelBusLight<T_METHOD, T_COLOR_FEATURE>{
      .output = neo_pixel,
      .state = make.state,
  };
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
Application::MakeNeoPixelBusLight<T_METHOD, T_COLOR_FEATURE> Application::make_neo_pixel_bus_rgbw_light(
    const std::string &name) {
  auto *neo_pixel = this->register_component(new light::NeoPixelRGBWLightOutput<T_METHOD, T_COLOR_FEATURE>());
  auto make = this->make_light_for_light_output(name, neo_pixel);

  return MakeNeoPixelBusLight<T_METHOD, T_COLOR_FEATURE>{
      .output = neo_pixel,
      .state = make.state,
  };
}
#endif

ESPHOME_NAMESPACE_END

/// namespace alias for backwards-compatability
namespace esphomelib = esphome;

#endif  // ESPHOME_APPLICATION_H
