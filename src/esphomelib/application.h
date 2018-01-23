//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_APPLICATION_H
#define ESPHOMELIB_APPLICATION_H

#include <vector>
#include <WiFiClient.h>
#include <esphomelib/output/ledc_output_component.h>
#include <esphomelib/output/pca9685_output_component.h>
#include <esphomelib/light/mqtt_json_light_component.h>
#include <esphomelib/input/gpio_binary_sensor_component.h>
#include <esphomelib/binary_sensor/mqtt_binary_sensor_component.h>
#include <esphomelib/sensor/mqtt_sensor_component.h>
#include <esphomelib/input/dht_component.h>
#include <esphomelib/light/light_output_component.h>
#include <esphomelib/switch_platform/mqtt_switch_component.h>
#include <esphomelib/switch_platform/switch.h>
#include <esphomelib/output/ir_transmitter_component.h>
#include <DallasTemperature.h>
#include <esphomelib/input/dallas_component.h>
#include <esphomelib/switch_platform/simple_switch.h>
#include "component.h"
#include "esphomelib/mqtt/mqtt_client_component.h"
#include "wifi_component.h"
#include "log_component.h"
#include "atx_component.h"
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/sensor/sensor.h"
#include "ota_component.h"

namespace esphomelib {

/** Application - This is the class that combines all components.
 *
 * Firstly, this class is used the register component
 */
class Application {
 public:
  Application();

  /** Set the name of the item that is running this app.
   *
   * Note: This will automatically be converted to lowercase_underscore.
   *
   * @param name The name of your app.
   */
  void set_name(const std::string &name);

  /** Initialize the log system.
   *
   * @param baud_rate The serial baud rate. Set to 0 to disable UART debugging.
   * @param mqtt_topic The MQTT debug topic. Set to "" to disable debugging via MQTT.
   * @param tx_buffer_size The size of the printf buffer.
   * @return The created and initialized LogComponent.
   */
  LogComponent *init_log(uint32_t baud_rate, const std::string &mqtt_topic, size_t tx_buffer_size = 512);

  /** Initialize the log system.
   *
   * Note: automatically enables MQTT debugging.
   *
   * @param baud_rate The serial baud rate. Set to 0 to disable UART debugging.
   * @return The created and initialized LogComponent.
   */
  LogComponent *init_log(uint32_t baud_rate = 115200);

  /** Initialize the WiFi system.
   *
   * Note: for advanced options, such as manual ip, use the return value.
   *
   * @param ssid The ssid of the network you want to connect to.
   * @param password The password of your network.
   * @return The WiFiComponent.
   */
  WiFiComponent *init_wifi(const std::string &ssid, const std::string &password);

  /** Initialize Over-the-Air updates.
   *
   * @return The OTAComponent. Use this to set advanced settings.
   */
  OTAComponent *init_ota();

  /** Initialize the MQTT client.
   *
   * @param address The address of your server.
   * @param port The port of your server.
   * @param username The username.
   * @param password The password. Empty for no password.
   * @param discovery_prefix The discovery prefix for home assistant. Leave empty for no discovery.
   * @return The MQTTClient. Use this to set advanced settings.
   */
  mqtt::MQTTClientComponent *init_mqtt(const std::string &address, uint16_t port,
                                       const std::string &username, const std::string &password,
                                       const std::string &discovery_prefix = "discovery");

  /** Initialize the MQTT client.
   *
   * @param address The address of your server.
   * @param username The username.
   * @param password The password. Empty for no password.
   * @param discovery_prefix The discovery prefix for home assistant. Leave empty for no discovery.
   * @return The MQTTClient. Use this to set advanced settings.
   */
  mqtt::MQTTClientComponent *init_mqtt(const std::string &address,
                                       const std::string &username, const std::string &password,
                                       const std::string &discovery_prefix = "discovery");

  /** Create a ATX power supply component that will automatically switch on and off.
   *
   * @param pin The pin the power supply is connected to.
   * @param enable_time The time (in ms) the power supply needs until it can provide high power when powering on.
   * @param keep_on_time The time (in ms) the power supply should stay on when it is not used.
   * @return The ATXComponent.
   */
  ATXComponent *make_atx(uint8_t pin, uint32_t enable_time = 20, uint32_t keep_on_time = 10000);

  // ======================= BINARY SENSOR =======================

  /// Create a GPIOBinarySensorComponent. Mostly for internal use.
  input::GPIOBinarySensorComponent *make_gpio_binary_sensor(uint8_t pin, uint8_t mode = INPUT,
                                                            binary_sensor::binary_callback_t callback = nullptr);

  /// Create a MQTTBinarySensorComponent. Mostly for internal use.
  binary_sensor::MQTTBinarySensorComponent *make_mqtt_binary_sensor(std::string friendly_name,
                                                                    std::string device_class);

  /// Connect a BinarySensor to a MQTTBinarySensorComponent. Mostly for internal use.
  void connect_binary_sensor_pair(binary_sensor::BinarySensor *binary_sensor,
                                  binary_sensor::MQTTBinarySensorComponent *mqtt);

  struct SimpleBinarySensor {
    input::GPIOBinarySensorComponent *gpio;
    binary_sensor::MQTTBinarySensorComponent *mqtt;
  };

  /** Create a simple GPIO binary sensor.
   *
   * Note: advanced options such as inverted input are available in the return value.
   *
   * @param friendly_name The friendly name that should be advertised. Leave empty for no automatic discovery.
   * @param device_class The Home Assistant <a href="https://home-assistant.io/components/binary_sensor/">device_class</a>.
   *                     or esphomelib::binary_sensor::device_class
   * @param pin The GPIO pin.
   */
  SimpleBinarySensor make_simple_gpio_binary_sensor(std::string friendly_name,
                                                    std::string device_class,
                                                    uint8_t pin);

  // ======================= SENSOR =======================
  /// Create a MQTTSensorComponent. Mostly for internal use.
  sensor::MQTTSensorComponent *make_mqtt_sensor(std::string friendly_name, std::string unit_of_measurement,
                                                Optional<uint32_t> expire_after = Optional<uint32_t>());

  /// Create a MQTTSensorComponent for the provided Sensor and connect them. Mostly for internal use.
  sensor::MQTTSensorComponent *make_mqtt_sensor_for(sensor::Sensor *sensor, std::string friendly_name,
                                                    Optional<uint32_t> expire_after = Optional<uint32_t>());

  struct MakeDHTComponent {
    input::DHTComponent *dht;
    sensor::MQTTSensorComponent *mqtt_temperature;
    sensor::MQTTSensorComponent *mqtt_humidity;
  };

  /** Create a DHT sensor component.
   *
   * Note: This method automatically applies a SlidingWindowMovingAverageFilter.
   *
   * @param pin The pin the DHT sensor is connected to.
   * @param temperature_friendly_name The name the temperature sensor should be advertised as. Leave empty for no
   *                                  automatic discovery.
   * @param humidity_friendly_name The name the humidity sensor should be advertised as. Leave empty for no
   *                                  automatic discovery.
   * @param check_interval The interval (in ms) the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  MakeDHTComponent make_dht_component(uint8_t pin,
                                      const std::string &temperature_friendly_name,
                                      const std::string &humidity_friendly_name,
                                      uint32_t check_interval = 5000);

  input::DallasComponent *make_dallas_component(OneWire *one_wire);

  input::DallasComponent *make_dallas_component(uint8_t pin);

  // ======================= OUTPUT =======================
  /** Create a ESP32 LEDC channel.
   *
   * @param pin The pin.
   * @param frequency The PWM frequency.
   * @param bit_depth The LEDC bit depth.
   * @return The LEDC component. Use this for advanced settings.
   */
  output::LEDCOutputComponent *make_ledc_component(uint8_t pin, float frequency = 1000.0f, uint8_t bit_depth = 12);

  /** Create a PCA9685 component.
   *
   * @param frequency The PWM frequency.
   * @param i2c_wire The i2c interface.
   * @return The PCA9685 component. Use this for advanced settings.
   */
  output::PCA9685OutputComponent *make_pca9685_component(float frequency, TwoWire &i2c_wire = Wire);

  // ======================= LIGHT =======================
  /// Create a MQTTJSONLightComponent. Mostly for internal use.
  light::MQTTJSONLightComponent *make_mqtt_light(const std::string &friendly_name, light::LightState *state);

  struct LightStruct {
    light::LinearLightOutputComponent *output;
    light::LightState *state;
    light::MQTTJSONLightComponent *mqtt;
  };

  /// Create and connect a MQTTJSONLightComponent to the provided light output component. Mostly for internal use.
  LightStruct connect_light(const std::string &friendly_name, light::LinearLightOutputComponent *out);

  /** Create a binary light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param binary The binary output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  LightStruct make_binary_light(const std::string &friendly_name, output::BinaryOutput *binary);

  /** Create a monochromatic light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param mono The output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  LightStruct make_monochromatic_light(const std::string &friendly_name, output::FloatOutput *mono);

  /** Create a RGB light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param red The red output channel.
   * @param green The green output channel.
   * @param blue The blue output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  LightStruct make_rgb_light(const std::string &friendly_name,
                             output::FloatOutput *red, output::FloatOutput *green, output::FloatOutput *blue);

  /** Create a RGBW light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param red The red output channel.
   * @param green The green output channel.
   * @param blue The blue output channel.
   * @param white The white output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  LightStruct make_rgbw_light(const std::string &friendly_name,
                              output::FloatOutput *red, output::FloatOutput *green, output::FloatOutput *blue,
                              output::FloatOutput *white);

  // ======================= SWITCH =======================
  /// Create a MQTTSwitchComponent. Mostly for internal use.
  switch_platform::MQTTSwitchComponent *make_mqtt_switch(const std::string &friendly_name);

  /// Connect the provided Switch to the MQTTSwitchComponent. Mostly for internal use.
  void connect_switch(switch_platform::Switch *switch_, switch_platform::MQTTSwitchComponent *mqtt);

  /** Create an IR transmitter.
   *
   * @param pin The pin the IR led is connected to.
   * @param carrier_duty_percent The duty cycle of the IR output. Decrease this if your LED gets hot.
   * @param clock_divider The clock divider for the rmt peripheral.
   * @return The IRTransmitterComponent. Use this for advanced settings.
   */
  output::IRTransmitterComponent *make_ir_transmitter(uint8_t pin,
                                                      uint8_t carrier_duty_percent = 50,
                                                      uint8_t clock_divider = output::DEFAULT_CLOCK_DIVIDER);

  struct SimpleGPIOSwitchStruct {
    switch_platform::SimpleSwitch *simple_switch;
    switch_platform::MQTTSwitchComponent *mqtt_switch;
  };

  SimpleGPIOSwitchStruct make_simple_gpio_switch(uint8_t pin, const std::string &friendly_name);

  /// Create a MQTTSwitchComponent for the provided Switch.
  switch_platform::MQTTSwitchComponent *make_mqtt_switch_for(const std::string &friendly_name,
                                                             switch_platform::Switch *switch_);

  // ======================= FUNCTIONS =======================

  template<class C>
  C *register_mqtt_component(C *c);

  /// Register the component in this Application instance.
  template<class C>
  C *register_component(C *c);

  /// Set up all the registered components. Call this at the end of your setup() function.
  void setup();

  /// Make a loop iteration. Call this in your loop() function.
  void loop();

  /// Generate a MQTT availability topic.
  std::string gen_availability_topic();

  WiFiComponent *get_wifi() const;
  mqtt::MQTTClientComponent *get_mqtt_client() const;

  /// Assert that name has been set.
  void assert_name() const;

  /// Get the name of this Application set by set_name().
  const std::string &get_name() const;

 protected:
  std::vector<Component *> components_;
  mqtt::MQTTClientComponent *mqtt_client_;
  WiFiComponent *wifi_;

  std::string name_;
};

/// Global storage of Application pointer - only one Application can exist.
extern Application *global_application;

template<class C>
C *Application::register_component(C *c) {
  Component *component = c;
  this->components_.push_back(component);
  return c;
}

template<class C>
C *Application::register_mqtt_component(C *c) {
  mqtt::MQTTComponent *component = c;
  component->set_availability(mqtt::Availability{
      .topic = this->gen_availability_topic(),
      .payload_available = "online",
      .payload_not_available = "offline"
  });
  return this->register_component(c);
}

} // namespace esphomelib

#endif //ESPHOMELIB_APPLICATION_H
