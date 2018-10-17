#ifndef ESPHOMELIB_SENSOR_CSE7766_H
#define ESPHOMELIB_SENSOR_CSE7766_H

#include "esphomelib/defines.h"

#ifdef USE_CSE7766

#include "esphomelib/component.h"
#include "esphomelib/uart_component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/sensor/sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using CSE7766VoltageSensor = EmptySensor<1, ICON_FLASH, UNIT_V>;
using CSE7766CurrentSensor = EmptySensor<1, ICON_FLASH, UNIT_A>;
using CSE7766PowerSensor = EmptySensor<1, ICON_FLASH, UNIT_W>;

class CSE7766Component : public Component, public UARTDevice {
 public:
  CSE7766Component(UARTComponent *parent);

  CSE7766VoltageSensor *make_voltage_sensor(const std::string &name);

  CSE7766CurrentSensor *make_current_sensor(const std::string &name);

  CSE7766PowerSensor *make_power_sensor(const std::string &name);

  void setup() override;
  void loop() override;
  float get_setup_priority() const override;

 protected:
  bool check_byte_(uint8_t byte, uint8_t index);
  void parse_data_();
  uint32_t get_24_bit_uint(uint8_t start_index);

  uint8_t raw_data_[24];
  uint8_t raw_data_index_{0};
  uint32_t last_transmission_{0};
  CSE7766VoltageSensor *voltage_{nullptr};
  CSE7766CurrentSensor *current_{nullptr};
  CSE7766PowerSensor *power_{nullptr};
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_CSE7766

#endif //ESPHOMELIB_SENSOR_CSE7766_H
