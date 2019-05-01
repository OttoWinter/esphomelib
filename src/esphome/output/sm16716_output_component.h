#ifndef ESPHOME_OUTPUT_SM16716_OUTPUT_COMPONENT_H
#define ESPHOME_OUTPUT_SM16716_OUTPUT_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_SM16716_OUTPUT

#include "esphome/output/float_output.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

/// SM16716 float output component.
class SM16716OutputComponent : public Component {
 public:
  class Channel;
  /** Construct the component.
   *
   * @param pin_data The data output pin, connected to the SM16716's DIN pin.
   * @param pin_clock The clock pin, connected to the SM16716's DCLK pin.
   * @param num_channels Total number of channels of the whole daisy chain.
   * @param num_chips Number of chips in the daisy chain.
   * @param update Update/reset duty data at boot (driver will keep
   *               configuration after powercycle)
   */
  SM16716OutputComponent(GPIOPin *pin_data, GPIOPin *pin_clock);

  /** Get a SM16716 output channel.
   *
   * @param channel The channel number. (0 is the closest channel)
   * @param power_supply The power supply that should be set for this channel.
   *                     Default: nullptr.
   * @param max_power The maximum power output of this channel. Each value will
   *                  be multiplied by this.
   * @return The new channel output component.
   */
  Channel *create_channel(uint8_t channel, PowerSupplyComponent *power_supply = nullptr, float max_power = 1.0f);

  /// Manually set the total number of channels. Defaults to 3.
  void set_num_channels(uint8_t num_channels);

  /// Manually set the number of chips. Defaults to 1.
  void set_num_chips(uint8_t num_chips);

  /// Setup the SM16716.
  void setup() override;

  void dump_config() override;

  /// HARDWARE setup_priority
  float get_setup_priority() const override;

  /// Send new values if they were updated.
  void loop() override;

  class Channel : public FloatOutput {
   public:
    Channel(SM16716OutputComponent *parent, uint8_t channel);

   protected:
    void write_state(float state) override;

    SM16716OutputComponent *parent_;
    uint8_t channel_;
  };

 protected:
  void set_channel_value_(uint8_t channel, uint8_t value);
  void write_bit_(bool);
  void write_byte_(uint8_t);

  GPIOPin *pin_data_;
  GPIOPin *pin_clock_;
  uint8_t num_channels_{3};
  uint8_t num_chips_{1};
  std::vector<uint8_t> pwm_amounts_;
  bool update_{true};
};

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_SM16716_OUTPUT

#endif  // ESPHOME_OUTPUT_SM16716_OUTPUT_COMPONENT_H
