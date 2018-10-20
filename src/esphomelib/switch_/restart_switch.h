#ifndef ESPHOMELIB_RESTART_SWITCH_H
#define ESPHOMELIB_RESTART_SWITCH_H

#include "esphomelib/switch_/switch.h"
#include "esphomelib/defines.h"

#ifdef USE_RESTART_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

/// A simple switch that restarts the device when triggered.
class RestartSwitch : public Switch {
 public:
  explicit RestartSwitch(const std::string &name);

  std::string icon() override;
 protected:
  void write_state(bool state) override;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_RESTART_SWITCH

#endif //ESPHOMELIB_RESTART_SWITCH_H
