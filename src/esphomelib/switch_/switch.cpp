#include "esphomelib/defines.h"

#ifdef USE_SWITCH

#include "esphomelib/switch_/switch.h"
#include "esphomelib/log.h"
#include "esphomelib/esppreferences.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch";

std::string Switch::icon() {
  return "";
}
Switch::Switch(const std::string &name)
  : Nameable(name), state(false) {

}

std::string Switch::get_icon() {
  if (this->icon_.has_value())
    return *this->icon_;
  return this->icon();
}

void Switch::set_icon(const std::string &icon) {
  this->icon_ = icon;
}
void Switch::turn_on() {
  ESP_LOGD(TAG, "'%s' Turning ON.", this->get_name().c_str());
  this->write_state(!this->inverted_);
}
void Switch::turn_off() {
  ESP_LOGD(TAG, "'%s' Turning OFF.", this->get_name().c_str());
  this->write_state(this->inverted_);
}
void Switch::toggle() {
  ESP_LOGD(TAG, "'%s' Toggling %s.", this->get_name().c_str(), this->state ? "OFF" : "ON");
  this->write_state(this->inverted_ == this->state);
}
float Switch::get_setup_priority() const {
  return setup_priority::HARDWARE - 1.0f;
}
optional<bool> Switch::get_initial_state() {
  this->rtc_ = global_preferences.make_preference(4, 2704004739UL);
  if (!this->rtc_.load())
    return {};
  bool initial_state = this->rtc_[0];
  return initial_state;
}
void Switch::publish_state(bool state) {
  this->state = state != this->inverted_;

  this->rtc_[0] = this->state ? 1 : 0;
  this->rtc_.save();
  this->state_callback_.call(this->state);
}
bool Switch::optimistic() {
  return false;
}

void Switch::add_on_state_callback(std::function<void(bool)> &&callback) {
  this->state_callback_.add(std::move(callback));
}
void Switch::set_inverted(bool inverted) {
  this->inverted_ = inverted;
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SWITCH
