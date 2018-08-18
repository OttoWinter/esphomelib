//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/defines.h"

#ifdef USE_GPIO_BINARY_SENSOR

#include "esphomelib/binary_sensor/gpio_binary_sensor_component.h"
#include "esphomelib/esphal.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.gpio";

void GPIOBinarySensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GPIO binary sensor '%s'...", this->name_.c_str());
  this->pin_->setup();
}

void GPIOBinarySensorComponent::loop() {
  this->publish_state(this->pin_->digital_read());
}

float GPIOBinarySensorComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

GPIOBinarySensorComponent::GPIOBinarySensorComponent(const std::string &name, GPIOPin *pin)
  : BinarySensor(name), pin_(pin) { }

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_GPIO_BINARY_SENSOR
