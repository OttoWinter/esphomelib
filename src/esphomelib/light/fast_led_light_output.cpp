#include "esphomelib/defines.h"

#ifdef USE_FAST_LED_LIGHT

#include "esphomelib/light/fast_led_light_output.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

static const char *TAG = "light.fast_led";

LightTraits FastLEDLightOutputComponent::get_traits() {
  return {true, true, false, true};
}
void FastLEDLightOutputComponent::write_state(LightState *state) {
  if (this->prevent_writing_leds_)
    return;

  float red, green, blue;
  state->current_values_as_rgb(&red, &green, &blue);
  CRGB crgb = CRGB(red * 255, green * 255, blue * 255);

  for (int i = 0; i < this->num_leds_; i++)
    this->leds_[i] = crgb;

  this->schedule_show();
}
void FastLEDLightOutputComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up FastLED light...");
  assert(this->controller_ != nullptr && "You need to add LEDs to this controller!");
  this->controller_->init();
  this->controller_->setLeds(this->leds_, this->num_leds_);
  this->controller_->setCorrection(this->correction_);
  if (!this->max_refresh_rate_.has_value()) {
    this->set_max_refresh_rate(this->controller_->getMaxRefreshRate());
  }
  ESP_LOGCONFIG(TAG, "    Max refresh rate: %u", *this->max_refresh_rate_);
}
void FastLEDLightOutputComponent::loop() {
  if (!this->next_show_)
    return;

  uint32_t now = micros();
  // protect from refreshing too often
  if (*this->max_refresh_rate_ != 0 && (now - this->last_refresh_) < *this->max_refresh_rate_) {
    return;
  }
  this->last_refresh_ = now;
  this->next_show_ = false;

  ESP_LOGVV(TAG, "Writing RGB values to bus...");

#ifdef USE_OUTPUT
  if (this->power_supply_ != nullptr) {
    bool is_on = false;
    for (int i = 0; i < this->num_leds_; i++) {
      if (bool(this->leds_[i])) {
        is_on = true;
        break;
      }
    }

    if (is_on && !this->has_requested_high_power_) {
      this->power_supply_->request_high_power();
      this->has_requested_high_power_ = true;
    }
    if (!is_on && this->has_requested_high_power_) {
      this->power_supply_->unrequest_high_power();
      this->has_requested_high_power_ = false;
    }
  }
#endif

  this->controller_->showLeds();
}
void FastLEDLightOutputComponent::schedule_show() {
  ESP_LOGVV(TAG, "Scheduling show...");
  this->next_show_ = true;
}
CLEDController &FastLEDLightOutputComponent::add_leds(CLEDController *controller, int num_leds) {
  assert(this->controller_ == nullptr && "FastLEDLightOutputComponent only supports one controller at a time.");

  this->controller_ = controller;
  this->num_leds_ = num_leds;
  this->leds_ = new CRGB[num_leds];
  this->effect_data_ = new uint8_t[num_leds];

  for (int i = 0; i < this->num_leds_; i++)
    this->leds_[i] = CRGB::Black;

  return *this->controller_;
}
CRGB *FastLEDLightOutputComponent::leds() const {
  return this->leds_;
}
CLEDController *FastLEDLightOutputComponent::get_controller() const {
  return this->controller_;
}
void FastLEDLightOutputComponent::set_max_refresh_rate(uint32_t interval_us) {
  this->max_refresh_rate_ = interval_us;
}
int FastLEDLightOutputComponent::size() const {
  return this->num_leds_;
}
void FastLEDLightOutputComponent::unprevent_writing_leds() {
  this->prevent_writing_leds_ = false;
}
void FastLEDLightOutputComponent::prevent_writing_leds() {
  this->prevent_writing_leds_ = true;
}
float FastLEDLightOutputComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
#ifdef USE_OUTPUT
void FastLEDLightOutputComponent::set_power_supply(PowerSupplyComponent *power_supply) {
  this->power_supply_ = power_supply;
}
CRGB &FastLEDLightOutputComponent::operator[](int index) const { return this->leds()[index]; }
CRGB *FastLEDLightOutputComponent::begin() { return &this->leds()[0]; }
CRGB *FastLEDLightOutputComponent::end() { return &this->leds()[this->size()]; }
uint8_t *FastLEDLightOutputComponent::effect_data() const {
  return this->effect_data_;
}
void FastLEDLightOutputComponent::set_correction(float red, float green, float blue) {
  this->correction_ = CRGB(red * 255, green * 255, blue * 255);
}

#endif

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_FAST_LED_LIGHT
