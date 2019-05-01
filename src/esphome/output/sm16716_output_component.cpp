// Based on:
//   - The SM16716 implementation from this repository.
//   - The Tasmota SM16716 implementation:
//     https://github.com/arendst/Sonoff-Tasmota/blob/master/sonoff/xdrv_04_light.ino
//   - This older implementation:
//     https://github.com/sowbug/sm16716
//   - The datasheet (in Chinese only):
//     https://github.com/sowbug/sm16716/blob/master/SM16716%20Datasheet%20%5BChinese%5D.pdf

#include "esphome/defines.h"

#ifdef USE_SM16716_OUTPUT

#include "esphome/log.h"
#include "esphome/output/sm16716_output_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

static const char *TAG = "output.sm16716";

SM16716OutputComponent::SM16716OutputComponent(GPIOPin *pin_data, GPIOPin *pin_clock)
    : pin_data_(pin_data), pin_clock_(pin_clock) {}

void SM16716OutputComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SM16716OutputComponent...");
  this->pin_data_->setup();
  this->pin_data_->digital_write(false);
  this->pin_clock_->setup();
  this->pin_clock_->digital_write(false);
  this->pwm_amounts_.resize(this->num_channels_, 0);
}
void SM16716OutputComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "SM16716:");
  LOG_PIN("  Data Pin: ", this->pin_data_);
  LOG_PIN("  Clock Pin: ", this->pin_clock_);
  ESP_LOGCONFIG(TAG, "  Total number of channels: %u", this->num_channels_);
  ESP_LOGCONFIG(TAG, "  Number of chips: %u", this->num_chips_);
}

void SM16716OutputComponent::loop() {
  if (!this->update_) {
    return;
  }

  for (uint8_t i = 0; i < 50; i++) {
    this->write_bit_(false);
  }

  // send 25 bits (1 start bit plus 24 data bits) for each chip
  for (uint8_t index = 0; index < this->num_channels_; index++) {
    // send a start bit initially and after every 3 channels
    if (index % 3 == 0) {
      this->write_bit_(true);
    }

    this->write_byte_(this->pwm_amounts_[index]);
  }

  // send a blank 25 bits to signal the end
  this->write_bit_(false);
  this->write_byte_(0);
  this->write_byte_(0);
  this->write_byte_(0);

  this->update_ = false;
}

SM16716OutputComponent::Channel *SM16716OutputComponent::create_channel(uint8_t channel,
                                                                        PowerSupplyComponent *power_supply,
                                                                        float max_power) {
  ESP_LOGV(TAG, "Getting channel %d...", channel);
  auto *c = new Channel(this, channel);
  c->set_power_supply(power_supply);
  c->set_max_power(max_power);
  return c;
}

float SM16716OutputComponent::get_setup_priority() const { return setup_priority::HARDWARE; }

void SM16716OutputComponent::set_channel_value_(uint8_t channel, uint8_t value) {
  ESP_LOGVV(TAG, "set channels %u to %u", channel, value);
  uint8_t index = this->num_channels_ - channel - 1;
  if (this->pwm_amounts_[index] != value) {
    this->update_ = true;
  }
  this->pwm_amounts_[index] = value;
}

void SM16716OutputComponent::write_bit_(bool value) {
  this->pin_data_->digital_write(value);
  this->pin_clock_->digital_write(true);
  this->pin_clock_->digital_write(false);
}

void SM16716OutputComponent::write_byte_(uint8_t data) {
  uint8_t mask;

  for (mask = 0x80; mask; mask >>= 1) {
    this->write_bit_(data & mask);
  }
}

void SM16716OutputComponent::set_num_channels(uint8_t num_channels) { this->num_channels_ = num_channels; }

void SM16716OutputComponent::set_num_chips(uint8_t num_chips) { this->num_chips_ = num_chips; }

SM16716OutputComponent::Channel::Channel(SM16716OutputComponent *parent, uint8_t channel)
    : FloatOutput(), parent_(parent), channel_(channel) {}

void SM16716OutputComponent::Channel::write_state(float state) {
  auto amount = uint8_t(state * 0xFF);
  this->parent_->set_channel_value_(this->channel_, amount);
}

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_SM16716_OUTPUT
