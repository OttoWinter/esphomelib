#include "esphomelib/defines.h"

#ifdef USE_SWITCH

#include "esphomelib/switch_/mqtt_switch_component.h"

#include <utility>

#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.mqtt";

MQTTSwitchComponent::MQTTSwitchComponent(switch_::Switch *switch_)
    : MQTTComponent(), switch_(switch_) {

}

void MQTTSwitchComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT switch '%s'", this->switch_->get_name().c_str());
  ESP_LOGCONFIG(TAG, "    Icon: '%s'", this->switch_->get_icon().c_str());
  if (this->switch_->optimistic()) {
    ESP_LOGCONFIG(TAG, "    Optimistic: YES");
  }

  this->subscribe(this->get_command_topic(), [&](const std::string &payload) {
    switch (parse_on_off(payload.c_str())) {
      case PARSE_ON:
        ESP_LOGD(TAG, "'%s' Turning ON.", this->friendly_name().c_str());
        this->switch_->turn_on();
        break;
      case PARSE_OFF:
        ESP_LOGD(TAG, "'%s' Turning OFF.", this->friendly_name().c_str());
        this->switch_->turn_off();
        break;
      case PARSE_TOGGLE:
        ESP_LOGD(TAG, "'%s' Toggling.", this->friendly_name().c_str());
        this->switch_->toggle();
        break;
      case PARSE_NONE:
      default:
        ESP_LOGW(TAG, "'%s': Received unknown status payload: %s", this->friendly_name().c_str(), payload.c_str());
        this->status_momentary_warning("state", 5000);
        break;
    }
  });
  this->switch_->add_on_state_callback([this](bool enabled){
    this->defer([this, enabled]() {
      this->publish_state(enabled);
    });
  });
}

std::string MQTTSwitchComponent::component_type() const {
  return "switch";
}
void MQTTSwitchComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->switch_->get_icon().empty())
    root["icon"] = this->switch_->get_icon();
  if (this->switch_->optimistic())
    root["optimistic"] = true;
}
void MQTTSwitchComponent::send_initial_state() {
  this->publish_state(this->switch_->state);
}
bool MQTTSwitchComponent::is_internal() {
  return this->switch_->is_internal();
}
std::string MQTTSwitchComponent::friendly_name() const {
  return this->switch_->get_name();
}
void MQTTSwitchComponent::publish_state(bool state) {
  const char *state_s = state ? "ON" : "OFF";
  ESP_LOGD(TAG, "'%s': Sending state %s", this->friendly_name().c_str(), state_s);
  this->send_message(this->get_state_topic(), state_s);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SWITCH
