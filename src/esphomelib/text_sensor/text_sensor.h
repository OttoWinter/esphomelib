#ifndef ESPHOMELIB_TEXT_SENSOR_TEXT_SENSOR_H
#define ESPHOMELIB_TEXT_SENSOR_TEXT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_TEXT_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/automation.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

class TextSensorStateTrigger;

#define LOG_TEXT_SENSOR(prefix, type, obj) \
    if (obj != nullptr) { \
      ESP_LOGCONFIG(TAG, prefix type " '%s'", obj->get_name().c_str()); \
      if (!obj->get_icon().empty()) { \
        ESP_LOGCONFIG(TAG, prefix "  Icon: '%s'", obj->get_icon().c_str()); \
      } \
      if (!obj->unique_id().empty()) { \
        ESP_LOGV(TAG, prefix "  Unique ID: '%s'", obj->unique_id().c_str()); \
      } \
    }

#ifdef USE_MQTT_TEXT_SENSOR
class MQTTTextSensor;
#endif

class TextSensor : public Nameable {
 public:
  explicit TextSensor(const std::string &name) : Nameable(name) {}

  void publish_state(std::string state);

  void set_icon(const std::string &icon);

  void add_on_state_callback(std::function<void(std::string)> callback);

  std::string state;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  std::string get_icon();

  virtual std::string icon();

  virtual std::string unique_id();

  TextSensorStateTrigger *make_state_trigger();

  bool has_state();

#ifdef USE_MQTT_TEXT_SENSOR
  MQTTTextSensor *get_mqtt() const;
  void set_mqtt(MQTTTextSensor *mqtt);
#endif

 protected:
  uint32_t hash_base_() override;

  CallbackManager<void(std::string)> callback_;
  optional<std::string> icon_;
  bool has_state_{false};
#ifdef USE_MQTT_TEXT_SENSOR
  MQTTTextSensor *mqtt_{nullptr};
#endif
};

class TextSensorStateTrigger : public Trigger<std::string> {
 public:
  explicit TextSensorStateTrigger(TextSensor *parent);
};

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#include "esphomelib/text_sensor/mqtt_text_sensor.h"

#endif //USE_TEXT_SENSOR

#endif //ESPHOMELIB_TEXT_SENSOR_TEXT_SENSOR_H
