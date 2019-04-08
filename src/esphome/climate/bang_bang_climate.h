#ifndef ESPHOME_CORE_BANG_BANG_CLIMATE_H
#define ESPHOME_CORE_BANG_BANG_CLIMATE_H

#include "esphome/defines.h"

#ifdef USE_BANG_BANG_CLIMATE

#include "esphome/component.h"
#include "esphome/climate/climate_device.h"
#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace climate {

struct BangBangClimateTargetTempConfig {
 public:
  BangBangClimateTargetTempConfig();
  BangBangClimateTargetTempConfig(float default_temperature_low, float default_temperature_high);

  float default_temperature_low{NAN};
  float default_temperature_high{NAN};
};

class BangBangClimate : public ClimateDevice, public Component {
 public:
  BangBangClimate(const std::string &name);
  void setup() override;

  void set_sensor(sensor::Sensor *sensor);
  Trigger<> *get_idle_trigger() const;
  Trigger<> *get_cool_trigger() const;
  void set_supports_cool(bool supports_cool);
  Trigger<> *get_heat_trigger() const;
  void set_supports_heat(bool supports_heat);
  void set_normal_config(const BangBangClimateTargetTempConfig &normal_config);
  void set_away_config(const BangBangClimateTargetTempConfig &away_config);

 protected:
  /// Override control to change settings of the climate device.
  void control(const ClimateCall &call) override;
  /// Change the away setting, will reset target temperatures to defaults.
  void change_away_(bool away);
  /// Return the traits of this controller.
  ClimateTraits traits() override;

  /// Re-compute the state of this climate controller.
  void compute_state_();

  /// Switch the climate device to the given climate mode.
  void switch_to_mode_(ClimateMode mode);

  /// The sensor used for getting the current temperature
  sensor::Sensor *sensor_{nullptr};
  /** The trigger to call when the controller should switch to idle mode.
   *
   * In idle mode, the controller is assumed to have both heating and cooling disabled.
   */
  Trigger<> *idle_trigger_;
  /** The trigger to call when the controller should switch to cooling mode.
   */
  Trigger<> *cool_trigger_;
  /** Whether the controller supports cooling.
   *
   * A false value for this attribute means that the controller has no cooling action
   * (for example a thermostat, where only heating and not-heating is possible).
   */
  bool supports_cool_{false};
  /** The trigger to call when the controller should switch to heating mode.
   *
   * A null value for this attribute means that the controller has no heating action
   * For example window blinds, where only cooling (blinds closed) and not-cooling
   * (blinds open) is possible.
   */
  Trigger<> *heat_trigger_{nullptr};
  bool supports_heat_{false};
  /** A reference to the trigger that was previously active.
   *
   * This is so that the previous trigger can be stopped before enabling a new one.
   */
  Trigger<> *prev_trigger_{nullptr};
  /** The climate mode that is currently active - for a `.mode = AUTO` this will
   * contain the actual mode the device
   *
   */
  ClimateMode internal_mode_{CLIMATE_MODE_OFF};

  BangBangClimateTargetTempConfig normal_config_{};
  bool supports_away_{false};
  BangBangClimateTargetTempConfig away_config_{};
};

} // namespace climate

ESPHOME_NAMESPACE_END

#endif //USE_BANG_BANG_CLIMATE

#endif //ESPHOME_CORE_BANG_BANG_CLIMATE_H
