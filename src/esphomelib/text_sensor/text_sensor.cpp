#include "esphomelib/defines.h"

#ifdef USE_TEXT_SENSOR

#include "esphomelib/text_sensor/text_sensor.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

static const char *TAG = "text_sensor.text_sensor";

void TextSensor::publish_state(std::string state) {
  this->state = state;
  this->has_state_ = true;
  ESP_LOGV(TAG, "'%s': Received new value %s", this->name_.c_str(), value.c_str());
  this->callback_.call(state);
}
void TextSensor::set_icon(const std::string &icon) {
  this->icon_ = icon;
}
void TextSensor::add_on_state_callback(std::function<void(std::string)> callback) {
  this->callback_.add(std::move(callback));
}
std::string TextSensor::get_icon() {
  if (this->icon_.has_value())
    return *this->icon_;
  return this->icon();
}
std::string TextSensor::icon() {
  return "";
}
std::string TextSensor::unique_id() {
  return "";
}
TextSensorStateTrigger *TextSensor::make_state_trigger() {
  return new TextSensorStateTrigger(this);
}
bool TextSensor::has_state() {
  return this->has_state_;
}

TextSensorStateTrigger::TextSensorStateTrigger(TextSensor *parent) {
  parent->add_on_state_callback([this](std::string value) {
    this->trigger(value);
  });
}

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEXT_SENSOR
