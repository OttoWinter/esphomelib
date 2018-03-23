//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_SWITCH_PLATFORM_SWITCH_H
#define ESPHOMELIB_SWITCH_PLATFORM_SWITCH_H

#include "esphomelib/binary_sensor/binary_sensor.h"

namespace esphomelib {

namespace switch_platform {

/** Base class for all switches.
 *
 * A switch is basically just a combination of a binary sensor (for reporting switch values)
 * and a write_state method that writes a state to the hardware.
 */
class Switch : public binary_sensor::BinarySensor {
 public:
  virtual void turn_on() = 0;
  virtual void turn_off() = 0;

  /** Override this to set the Home Assistant icon for this switch.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this switch, for example "mdi:fan".
   */
  virtual std::string icon();
};

} // namespace switch_platform

} // namespace esphomelib

#endif //ESPHOMELIB_SWITCH_PLATFORM_SWITCH_H
