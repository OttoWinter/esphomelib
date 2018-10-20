#include "esphomelib/defines.h"

#ifdef USE_WIFI_SIGNAL_SENSOR

#include "esphomelib/sensor/wifi_signal_sensor.h"
#ifdef ARDUINO_ARCH_ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

WiFiSignalSensor::WiFiSignalSensor(const std::string &name, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval) {

}
void WiFiSignalSensor::update() {
  this->publish_state(WiFi.RSSI());
}
std::string WiFiSignalSensor::unit_of_measurement() {
  return "dB";
}
std::string WiFiSignalSensor::icon() {
  return "mdi:wifi";
}
int8_t WiFiSignalSensor::accuracy_decimals() {
  return 0;
}
std::string WiFiSignalSensor::unique_id() {
  return get_mac_address() + "-wifisignal";
}
float WiFiSignalSensor::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_WIFI_SIGNAL_SENSOR
