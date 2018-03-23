//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_SENSOR_SENSOR_H
#define ESPHOMELIB_SENSOR_SENSOR_H

#include <functional>

#include "esphomelib/component.h"

namespace esphomelib {

namespace sensor {

using sensor_callback_t = std::function<void(float, int8_t)>;

/** Base-class for all sensors.
 *
 * A sensor has unit of measurement and can use push_new_value to send out a new value with the specified accuracy.
 */
class Sensor {
 public:
  /** Initialize this sensor with the given update interval in ms.
   *
   * If your Sensor isn't a polling sensor, you can pass 0 to the update interval.
   *
   * @param update_interval The update interval in ms.
   */
  explicit Sensor(uint32_t update_interval);

  /// Manually set the update interval in ms that the sensor should update its values.
  virtual void set_update_interval(uint32_t update_interval);

  // ========== OVERRIDE METHODS ==========
  // (You'll only need this when creating your own custom sensor)
  /** Push a new value to the MQTT front-end.
   *
   * Note that you should publish the raw value here, i.e. without any rounding as the user
   * can later override this accuracy.
   *
   * @param value The floating point value.
   * @param accuracy_decimals The accuracy in decimal points. The user can customize this.
   */
  void push_new_value(float value, int8_t accuracy_decimals);

  /** Override this to set the Home Assistant unit of measurement for this sensor.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this sensor, for example "°C".
   */
  virtual std::string unit_of_measurement();

  /** Override this to set the Home Assistant icon for this sensor.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this sensor, for example "mdi:battery".
   */
  virtual std::string icon();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get the update interval in ms of this sensor.
  virtual uint32_t get_update_interval() const;

  /// The MQTT sensor class uses this to register itself as a listener for new values.
  void set_new_value_callback(sensor_callback_t callback);

 protected:
  sensor_callback_t callback_{nullptr};
  uint32_t update_interval_{0};
};

class TemperatureSensor : public Sensor {
 public:
  explicit TemperatureSensor(uint32_t update_interval);
  std::string unit_of_measurement() override;
  std::string icon() override;
};

class HumiditySensor : public Sensor {
 public:
  explicit HumiditySensor(uint32_t update_interval);
  std::string unit_of_measurement() override;
  std::string icon() override;
};

class VoltageSensor : public Sensor {
 public:
  explicit VoltageSensor(uint32_t update_interval);
  std::string unit_of_measurement() override;
  std::string icon() override;
};

class DistanceSensor : public Sensor {
 public:
  explicit DistanceSensor(uint32_t update_interval);
  std::string unit_of_measurement() override;
  std::string icon() override;
};

} // namespace sensor

} // namespace esphomelib


#endif //ESPHOMELIB_SENSOR_SENSOR_H
