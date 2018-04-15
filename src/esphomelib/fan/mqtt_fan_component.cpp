//
// Created by Otto Winter on 29.12.17.
//

#include "esphomelib/fan/mqtt_fan_component.h"

#include "esphomelib/log.h"

#ifdef USE_FAN

namespace esphomelib {

namespace fan {

static const char *TAG = "fan.mqtt";

MQTTFanComponent::MQTTFanComponent(FanState *state)
    : MQTTComponent(), state_(state) {
  assert(this->state_ != nullptr);
}

FanState *MQTTFanComponent::get_state() const {
  return this->state_;
}
std::string MQTTFanComponent::component_type() const {
  return "fan";
}
void MQTTFanComponent::setup() {
  ESP_LOGD(TAG, "Setting up MQTT fan...");

  this->subscribe(this->get_command_topic(), [this](const std::string &payload) {
    if (strcasecmp(payload.c_str(), "ON") == 0) {
      ESP_LOGD(TAG, "Turning Fan ON.");
      this->state_->set_state(true);
    } else if (strcasecmp(payload.c_str(), "OFF") == 0) {
      ESP_LOGD(TAG, "Turning Fan OFF.");
      this->state_->set_state(false);
    }
  });

  if (this->state_->get_traits().supports_oscillation()) {
    this->subscribe(this->get_oscillation_command_topic(), [this](const std::string &payload) {
      auto val = parse_on_off(payload.c_str(), "oscillate_on", "oscillate_off");
      if (val.defined) {
        ESP_LOGW(TAG, "Unknown Oscillation Payload %s", payload.c_str());
        return;
      }
      this->state_->set_oscillating(val.value);
    });
  }

  if (this->state_->get_traits().supports_speed()) {
    this->subscribe(this->get_speed_command_topic(), [this](const std::string &payload) {
      this->state_->set_speed(payload.c_str());
    });
  }

  this->state_->add_on_receive_backend_state_callback([this]() { this->next_send_ = true; });

  this->state_->load_from_preferences();
}
void MQTTFanComponent::set_custom_oscillation_command_topic(const std::string &topic) {
  this->set_custom_topic("oscillation/command", topic);
}
void MQTTFanComponent::set_custom_oscillation_state_topic(const std::string &topic) {
  this->set_custom_topic("oscillation/state", topic);
}
void MQTTFanComponent::set_custom_speed_command_topic(const std::string &topic) {
  this->set_custom_topic("speed/command", topic);
}
void MQTTFanComponent::set_custom_speed_state_topic(const std::string &topic) {
  this->set_custom_topic("speed/state", topic);
}
const std::string MQTTFanComponent::get_oscillation_command_topic() const {
  return this->get_topic_for("oscillation/command");
}
const std::string MQTTFanComponent::get_oscillation_state_topic() const {
  return this->get_topic_for("oscillation/state");
}
const std::string MQTTFanComponent::get_speed_command_topic() const {
  return this->get_topic_for("speed/command");
}
const std::string MQTTFanComponent::get_speed_state_topic() const {
  return this->get_topic_for("speed/state");
}
void MQTTFanComponent::loop() {
  if (this->next_send_)
    this->send_state();
}
void MQTTFanComponent::send_state() {
  ESP_LOGD(TAG, "Sending state.");
  this->send_message(this->get_state_topic(), this->state_->get_state() ? "ON" : "OFF");
  if (this->state_->get_traits().supports_oscillation())
    this->send_message(this->get_oscillation_state_topic(),
                       this->state_->is_oscillating() ? "oscillate_on" : "oscillate_off");
  if (this->state_->get_traits().supports_speed()) {
    std::string payload;
    switch (this->state_->get_speed()) {
      case FanState::SPEED_OFF: {
        payload = "off";
        break;
      }
      case FanState::SPEED_LOW: {
        payload = "low";
        break;
      }
      case FanState::SPEED_MEDIUM: {
        payload = "medium";
        break;
      }
      case FanState::SPEED_HIGH: {
        payload = "high";
        break;
      }
    }
    this->send_message(this->get_speed_state_topic(), payload);
  }
  this->state_->save_to_preferences();
  this->next_send_ = false;
}
std::string MQTTFanComponent::friendly_name() const {
  return this->state_->get_name();
}
void MQTTFanComponent::send_discovery(JsonBuffer &buffer, JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (this->state_->get_traits().supports_oscillation()) {
    root["oscillation_command_topic"] = this->get_oscillation_command_topic();
    root["oscillation_state_topic"] = this->get_oscillation_state_topic();
  }
  if (this->state_->get_traits().supports_speed()) {
    root["speed_command_topic"] = this->get_speed_command_topic();
    root["speed_state_topic"] = this->get_speed_state_topic();
  }
}

} // namespace fan

} // namespace esphomelib

#endif //USE_FAN
