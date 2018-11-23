#include "esphomelib/defines.h"

#ifdef USE_TOTAL_DAILY_ENERGY_SENSOR

#include "esphomelib/sensor/total_daily_energy.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

void TotalDailyEnergy::setup() {
  this->pref_ = global_preferences.make_preference<float>(1436924412UL, this->name_);

  float recovered;
  if (this->pref_.load(&recovered)) {
    this->publish_state_and_save_(recovered);
  } else {
    this->publish_state_and_save_(0);
  }
  this->last_update_ = millis();

  auto f = std::bind(&TotalDailyEnergy::process_new_state_, this, std::placeholders::_1);
  this->parent_->add_on_state_callback(f);
}
float TotalDailyEnergy::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
uint32_t TotalDailyEnergy::update_interval() {
  return this->parent_->update_interval();
}
std::string TotalDailyEnergy::unit_of_measurement() {
  return this->parent_->get_unit_of_measurement() + "h";
}
std::string TotalDailyEnergy::icon() {
  return this->parent_->get_icon();
}
int8_t TotalDailyEnergy::accuracy_decimals() {
  return this->parent_->get_accuracy_decimals() + 1;
}
void TotalDailyEnergy::process_new_state_(float state) {
  if (isnan(state))
    return;
  const uint32_t now = millis();
  float delta_hours = (now - this->last_update_) / 1000.0f / 60.0f / 60.0f;
  this->last_update_ = now;
  this->publish_state_and_save_(this->total_energy_ + state * delta_hours);
}
void TotalDailyEnergy::loop() {
  auto t = this->time_->now();
  if (!t.is_valid())
    return;

  if (this->last_day_of_year_ == 0) {
    this->last_day_of_year_ = t.day_of_year;
    return;
  }

  if (t.day_of_year != this->last_day_of_year_) {
    this->last_day_of_year_ = t.day_of_year;
    this->total_energy_ = 0;
    this->publish_state_and_save_(0);
  }
}
void TotalDailyEnergy::publish_state_and_save_(float state) {
  this->pref_.save(&state);
  this->total_energy_ = state;
  this->publish_state(state);
}
TotalDailyEnergy::TotalDailyEnergy(const std::string &name, time::RealTimeClockComponent *time, Sensor *parent)
    : Sensor(name), time_(time), parent_(parent) {
  this->clear_filters();
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TOTAL_DAILY_ENERGY_SENSOR
