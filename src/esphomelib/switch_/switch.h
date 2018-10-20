#ifndef ESPHOMELIB_SWITCH_SWITCH_H
#define ESPHOMELIB_SWITCH_SWITCH_H

#include "esphomelib/defines.h"

#ifdef USE_SWITCH

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/component.h"
#include "esphomelib/automation.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

template<typename T>
class ToggleAction;
template<typename T>
class TurnOffAction;
template<typename T>
class TurnOnAction;

/** Base class for all switches.
 *
 * A switch is basically just a combination of a binary sensor (for reporting switch values)
 * and a write_state method that writes a state to the hardware.
 */
class Switch : public Component, public Nameable {
 public:
  explicit Switch(const std::string &name);

  /** Publish a state to the front-end from the back-end.
   *
   * The input value is inverted if applicable. Then the internal value member is set and
   * finally the callbacks are called.
   *
   * @param state The new state.
   */
  void publish_state(bool state);

  union {
    /// The current reported state of the binary sensor.
    bool state;
    ESPDEPRECATED(".value is deprecated, please use .state instead") bool value;
  };

  /** Turn this switch on. This is called by the front-end.
   *
   * For implementing switches, please override write_state.
   */
  void turn_on();
  /** Turn this switch off. This is called by the front-end.
   *
   * For implementing switches, please override write_state.
   */
  void turn_off();
  /** Toggle this switch. This is called by the front-end.
   *
   * For implementing switches, please override write_state.
   */
  void toggle();

  /** Set whether the state should be treated as inverted.
   *
   * To the developer and user an inverted switch will act just like a non-inverted one.
   * In particular, the only thing that's changed by this is the value passed to
   * write_state and the state in publish_state. The .state member variable and
   * turn_on/turn_off/toggle remain unaffected.
   *
   * @param inverted Whether to invert this switch.
   */
  void set_inverted(bool inverted);

  /// Set the icon for this switch. "" for no icon.
  void set_icon(const std::string &icon);

  /// Get the icon for this switch. Using icon() if not manually set
  std::string get_icon();

  template<typename T>
  ToggleAction<T> *make_toggle_action();
  template<typename T>
  TurnOffAction<T> *make_turn_off_action();
  template<typename T>
  TurnOnAction<T> *make_turn_on_action();

  /** Set callback for state changes.
   *
   * @param callback The void(bool) callback.
   */
  void add_on_state_callback(std::function<void(bool)> &&callback);

  float get_setup_priority() const override;

  void setup_() override;

  /** Return whether this switch is optimistic - i.e. if both the ON/OFF actions should be displayed in Home Assistant
   * because the real state is unknown.
   *
   * Defaults to false.
   */
  virtual bool optimistic();

  /// Subclasses can override this to prevent the switch from automatically restoring the state.
  virtual bool do_restore_state();

 protected:
  /** Write the given state to hardware. You should implement this
   * abstract method if you want to create your own switch.
   *
   * In the implementation of this method, you should also call
   * publish_state to acknowledge that the state was written to the hardware.
   *
   * @param state The state to write. Inversion is already applied if user specified it.
   */
  virtual void write_state(bool state) = 0;

  /** Override this to set the Home Assistant icon for this switch.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this switch, for example "mdi:fan".
   */
  virtual std::string icon();

  optional<std::string> icon_{}; ///< The icon shown here. Not set means use default from switch. Empty means no icon.

  CallbackManager<void(bool)> state_callback_{};
  bool inverted_{false};
};

template<typename T>
class TurnOnAction : public Action<T> {
 public:
  explicit TurnOnAction(Switch *a_switch);

  void play(T x) override;

 protected:
  Switch *switch_;
};

template<typename T>
class TurnOffAction : public Action<T> {
 public:
  explicit TurnOffAction(Switch *a_switch);

  void play(T x) override;

 protected:
  Switch *switch_;
};

template<typename T>
class ToggleAction : public Action<T> {
 public:
  explicit ToggleAction(Switch *a_switch);

  void play(T x) override;

 protected:
  Switch *switch_;
};

// =============== TEMPLATE DEFINITIONS ===============

template<typename T>
TurnOnAction<T>::TurnOnAction(Switch *a_switch) : switch_(a_switch) {}

template<typename T>
void TurnOnAction<T>::play(T x) {
  this->switch_->turn_on();
  this->play_next(x);
}

template<typename T>
TurnOffAction<T>::TurnOffAction(Switch *a_switch) : switch_(a_switch) {}

template<typename T>
void TurnOffAction<T>::play(T x) {
  this->switch_->turn_off();
  this->play_next(x);
}

template<typename T>
ToggleAction<T>::ToggleAction(Switch *a_switch) : switch_(a_switch) {}

template<typename T>
void ToggleAction<T>::play(T x) {
  this->switch_->toggle();
  this->play_next(x);
}

template<typename T>
ToggleAction<T> *Switch::make_toggle_action() {
  return new ToggleAction<T>(this);
}

template<typename T>
TurnOffAction<T> *Switch::make_turn_off_action() {
  return new TurnOffAction<T>(this);
}

template<typename T>
TurnOnAction<T> *Switch::make_turn_on_action() {
  return new TurnOnAction<T>(this);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SWITCH

#endif //ESPHOMELIB_SWITCH_SWITCH_H
