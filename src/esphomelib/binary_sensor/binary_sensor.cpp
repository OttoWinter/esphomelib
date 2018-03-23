//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/binary_sensor/binary_sensor.h"

namespace esphomelib {

namespace binary_sensor {

static const char *TAG = "binary_sensor::binary_sensor";

void BinarySensor::set_on_new_state_callback(binary_callback_t callback) {
  this->new_state_callback_ = std::move(callback);
}

void BinarySensor::publish_state(bool state) {
  this->new_state_callback_(state != this->inverted_);
}
bool BinarySensor::is_inverted() const {
  return this->inverted_;
}
void BinarySensor::set_inverted(bool inverted) {
  this->inverted_ = inverted;
}

} // namespace binary_sensor

} // namespace esphomelib
