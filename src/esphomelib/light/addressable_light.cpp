#include "esphomelib/defines.h"

#ifdef USE_LIGHT

#include "esphomelib/light/addressable_light.h"
#include "esphomelib/log.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

ESPColor HOT ESPColor::random_color() {
  uint32_t rand = random_uint32();
  uint8_t r = rand >> 16;
  uint8_t g = rand >> 8;
  uint8_t b = rand >> 0;
  const uint16_t max_ = std::max(r, std::max(g, b));
  return ESPColor(
      uint8_t((uint16_t(r) * 255U / max_)),
      uint8_t((uint16_t(g) * 255U / max_)),
      uint8_t((uint16_t(b) * 255U / max_))
  );
}

// based on FastLED's hsv rainbow to rgb
ESPColor HOT ESPHSVColor::to_rgb() const {
  uint8_t hue = this->hue;
  uint8_t sat = this->saturation;
  uint8_t val = this->value;
  // upper 3 hue bits are for branch selection, lower 5 are for values
  uint8_t offset8 = (hue & 0x1F) << 3; // 0..248
  // third of the offset, 255/3 = 85 (actually only up to 82; 164)
  uint8_t third = esp_scale8(offset8, 85);
  uint8_t two_thirds = esp_scale8(offset8, 170);
  ESPColor rgb(255, 255, 255, 0);
  switch (hue >> 5) {
    case 0b000:
      rgb.r = 255 - third;
      rgb.g = third;
      rgb.b = 0;
      break;
    case 0b001:
      rgb.r = 171;
      rgb.g = 85 + third;
      rgb.b = 0;
      break;
    case 0b010:
      rgb.r = 171 - two_thirds;
      rgb.g = 170 + two_thirds;
      rgb.b = 0;
      break;
    case 0b011:
      rgb.r = 0;
      rgb.g = 255 - third;
      rgb.b = third;
      break;
    case 0b100:
      rgb.r = 0;
      rgb.g = 171 - two_thirds;
      rgb.b = 85 + two_thirds;
      break;
    case 0b101:
      rgb.r = third;
      rgb.g = 0;
      rgb.b = 255 - third;
      break;
    case 0b110:
      rgb.r = 85 + third;
      rgb.g = 0;
      rgb.b = 171 - third;
      break;
    case 0b111:
      rgb.r = 170 + third;
      rgb.g = 0;
      rgb.b = 85 - third;
      break;
    default:
      break;
  }
  // Scale down colors if we're desaturated at all
  // and add the brightness_floor to r, g, and b.
  if (sat == 0) {
    rgb.r = 255;
    rgb.b = 255;
    rgb.g = 255;
  } else if (sat != 255) {
    // (r,g,b) = (r,g,b) * sat + (1 - sat)^2
    rgb *= sat;
    uint8_t desat = 255 - sat;
    rgb += esp_scale8(desat, desat);
  }
  // Now scale everything down if we're at value < 255.
  if (val != 255) {
    // (r,g,b) = (r,g,b) * val^2
    rgb *= esp_scale8(val, val);
  }
  return rgb;
}

ESPColorCorrection::ESPColorCorrection()
  : max_brightness_(255, 255, 255, 255) {
}

void ESPColorCorrection::set_local_brightness(uint8_t local_brightness) {
  this->local_brightness_ = local_brightness;
}

void ESPColorCorrection::set_max_brightness(const ESPColor &max_brightness) {
  this->max_brightness_ = max_brightness;
}

void ESPColorCorrection::calculate_gamma_table(float gamma) {
  for (uint16_t i = 0; i < 256; i++) {
    // corrected = val ^ gamma
    uint8_t corrected = roundf(255.0f * gamma_correct(i / 255.0f, gamma));
    this->gamma_table_[i] = corrected;
  }
  if (gamma == 0.0f) {
    for (uint16_t i = 0; i < 256; i++)
      this->gamma_reverse_table_[i] = i;
    return;
  }
  for (uint16_t i = 0; i < 256; i++) {
    // val = corrected ^ (1/gamma)
    uint8_t uncorrected = roundf(255.0f * powf(i / 255.0f, 1.0f / gamma));
    this->gamma_reverse_table_[i] = uncorrected;
  }
}

AddressableLight::AddressableLight() = default;

bool AddressableLight::is_effect_active() const {
  return this->effect_active_;
}

void AddressableLight::set_effect_active(bool effect_active) {
  this->effect_active_ = effect_active;
}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT