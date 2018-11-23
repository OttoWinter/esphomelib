#ifndef ESPHOMELIB_VERSION_TEXT_SENSOR_H
#define ESPHOMELIB_VERSION_TEXT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_VERSION_TEXT_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/text_sensor/text_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

class VersionTextSensor : public TextSensor, public Component {
 public:
  explicit VersionTextSensor(const std::string &name);
  void setup() override;
  float get_setup_priority() const override;
  std::string icon() override;
  std::string unique_id() override;
};

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_VERSION_TEXT_SENSOR

#endif //ESPHOMELIB_VERSION_TEXT_SENSOR_H
