#ifndef ESPHOMELIB_STATUS_BINARY_SENSOR_H
#define ESPHOMELIB_STATUS_BINARY_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_STATUS_BINARY_SENSOR

#include "esphomelib/binary_sensor/binary_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

/** Simple binary sensor that reports the online/offline state of the node using MQTT
 *
 * Most of the magic doesn't happen here, but in Application.make_status_binary_sensor.
 */
class StatusBinarySensor : public BinarySensor {
 public:
  /// Construct the status binary sensor
  explicit StatusBinarySensor(const std::string &name);

 protected:
  /// "connectivity" device class.
  std::string device_class() override;
};

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_STATUS_BINARY_SENSOR

#endif //ESPHOMELIB_STATUS_BINARY_SENSOR_H
