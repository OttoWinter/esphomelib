#ifndef ESPHOMELIB_SWITCH_TEMPLATE_SWITCH_H
#define ESPHOMELIB_SWITCH_TEMPLATE_SWITCH_H

#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_SWITCH

#include "esphomelib/switch_/switch.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

class TemplateSwitch : public Switch {
 public:
  explicit TemplateSwitch(const std::string &name);

  void set_state_lambda(std::function<optional<bool>()> &&f);
  Trigger<NoArg> *get_turn_on_trigger() const;
  Trigger<NoArg> *get_turn_off_trigger() const;
  void set_optimistic(bool optimistic);
  void loop() override;

  float get_setup_priority() const override;

  bool do_restore_state() override;

 protected:
  bool optimistic() override;

  void write_state(bool state) override;

  optional<std::function<optional<bool>()>> f_;
  bool optimistic_{false};
  optional<bool> last_state_{};
  Trigger<NoArg> *turn_on_trigger_;
  Trigger<NoArg> *turn_off_trigger_;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_SWITCH

#endif //ESPHOMELIB_SWITCH_TEMPLATE_SWITCH_H
