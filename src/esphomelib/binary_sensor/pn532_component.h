//
//  pn532_component.h
//  esphomelib
//
//  Created by Otto Winter on 15.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_PN_532_COMPONENT_H
#define ESPHOMELIB_PN_532_COMPONENT_H

#include "esphomelib/i2c_component.h"
#include "esphomelib/component.h"
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/spi_component.h"
#include "esphomelib/defines.h"

#include <vector>

#ifdef USE_PN532

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

class PN532BinarySensor;

class PN532Component : public PollingComponent, public SPIDevice {
 public:
  PN532Component(SPIComponent *parent, GPIOPin *cs, uint32_t update_interval = 1000);

  void setup() override;

  void update() override;
  float get_setup_priority() const override;

  void loop() override;

  PN532BinarySensor *make_tag(const std::string &name, const std::vector<uint8_t> &uid);

 protected:
  bool msb_first() override;

 protected:
  void pn532_write_command_(uint8_t len);
  bool pn532_write_command_check_ack_(uint8_t len, bool ignore = false);

  void pn532_read_data_(uint8_t len);

  bool is_ready();

  bool read_ack();

  uint8_t buffer_[32];
  bool requested_read_{false};
  std::vector<PN532BinarySensor *> binary_sensors_;
  std::vector<uint8_t> last_uid_;
};

class PN532BinarySensor : public binary_sensor::BinarySensor {
 public:
  PN532BinarySensor(const std::string &name, const std::vector<uint8_t> &uid, uint32_t update_interval);

  bool process(uint8_t *data, uint8_t len);
 protected:
  std::vector<uint8_t> uid_;
};

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_PN532

#endif //ESPHOMELIB_PN_532_COMPONENT_H
