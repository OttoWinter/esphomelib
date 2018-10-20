#include "esphomelib/defines.h"

#ifdef USE_SENSOR

#include "esphomelib/sensor/mqtt_sensor_component.h"

#include "esphomelib/espmath.h"
#include "esphomelib/log.h"
#include "esphomelib/component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.mqtt";

MQTTSensorComponent::MQTTSensorComponent(Sensor *sensor)
    : MQTTComponent(), sensor_(sensor) {

}

void MQTTSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT Sensor '%s'...", this->sensor_->get_name().c_str());
  if (this->get_expire_after() > 0) {
    ESP_LOGCONFIG(TAG, "    Expire After: %us", this->get_expire_after() / 1000);
  }
  ESP_LOGCONFIG(TAG, "    Unit of Measurement: '%s'", this->sensor_->get_unit_of_measurement().c_str());
  ESP_LOGCONFIG(TAG, "    Accuracy Decimals: %i", this->sensor_->get_accuracy_decimals());
  ESP_LOGCONFIG(TAG, "    Icon: '%s'", this->sensor_->get_icon().c_str());
  if (!this->sensor_->unique_id().empty()) {
    ESP_LOGCONFIG(TAG, "    Unique ID: '%s'", this->sensor_->unique_id().c_str());
  }

  auto f = std::bind(&MQTTSensorComponent::publish_state, this, std::placeholders::_1);
  this->sensor_->add_on_state_callback(f);
}

std::string MQTTSensorComponent::component_type() const {
  return "sensor";
}

uint32_t MQTTSensorComponent::get_expire_after() const {
  if (this->expire_after_.has_value()) {
    return *this->expire_after_;
  } else {
    return this->sensor_->calculate_expected_filter_update_interval() * 3;
  }
}
void MQTTSensorComponent::set_expire_after(uint32_t expire_after) {
  this->expire_after_ = expire_after;
}
void MQTTSensorComponent::disable_expire_after() {
  this->expire_after_ = 0;
}
std::string MQTTSensorComponent::friendly_name() const {
  return this->sensor_->get_name();
}
void MQTTSensorComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->sensor_->get_unit_of_measurement().empty())
    root["unit_of_measurement"] = this->sensor_->get_unit_of_measurement();

  if (this->get_expire_after() > 0)
    root["expire_after"] = this->get_expire_after() / 1000;

  if (!this->sensor_->get_icon().empty())
    root["icon"] = this->sensor_->get_icon();

  config.command_topic = false;
}
void MQTTSensorComponent::send_initial_state() {
  if (this->sensor_->has_state())
    this->publish_state(this->sensor_->state);
}
bool MQTTSensorComponent::is_internal() {
  return this->sensor_->is_internal();
}
void MQTTSensorComponent::publish_state(float value) {
  int8_t accuracy = this->sensor_->get_accuracy_decimals();
  ESP_LOGD(TAG, "'%s': Pushing out value %f with %d decimals of accuracy",
           this->sensor_->get_name().c_str(), value, accuracy);
  this->send_message(this->get_state_topic(), value_accuracy_to_string(value, accuracy));
}
std::string MQTTSensorComponent::unique_id() {
  return this->sensor_->unique_id();
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SENSOR
