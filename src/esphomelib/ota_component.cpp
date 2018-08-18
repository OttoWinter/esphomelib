//
// Created by Otto Winter on 03.12.17.
//

#include "esphomelib/defines.h"

#ifdef USE_OTA

#include "esphomelib/ota_component.h"
#include "esphomelib/log.h"
#include "esphomelib/esppreferences.h"
#include "esphomelib/helpers.h"
#include "esphomelib/wifi_component.h"
#include "esphomelib/status_led.h"
#include "esphomelib/defines.h"
#include <ArduinoOTA.h>

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "ota";
#ifdef ARDUINO_ARCH_ESP32
  static const char *PREF_TAG = "ota"; ///< Tag for preferences.
  static const char *PREF_SAFE_MODE_COUNTER_KEY = "safe_mode";
#endif

void OTAComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up OTA...");
  ESP_LOGCONFIG(TAG, "    port: %u", this->port_);
  this->server_ = new WiFiServer(this->port_);
  this->server_->begin();

  if (!this->hostname_.empty()) {
    ESP_LOGCONFIG(TAG, "    hostname: '%s'", this->hostname_.c_str());
    ArduinoOTA.setHostname(this->hostname_.c_str());
  }
  ArduinoOTA.setPort(this->port_);
  switch (this->auth_type_) {
    case PLAINTEXT: {
      ArduinoOTA.setPassword(this->password_.c_str());
      break;
    }
#if ARDUINO > 20300
    case HASH: {
      ArduinoOTA.setPasswordHash(this->password_.c_str());
      break;
    }
#endif
    case OPEN: {}
    default: break;
  }

  ArduinoOTA.onStart([this]() {
    ESP_LOGI(TAG, "OTA starting...");
    this->ota_triggered_ = true;
    this->at_ota_progress_message_ = 0;
    this->status_set_warning();
#ifdef USE_STATUS_LED
    global_state |= STATUS_LED_WARNING;
#endif
  });
  ArduinoOTA.onEnd([&]() {
    ESP_LOGI(TAG, "OTA update finished!");
    this->status_clear_warning();
    delay(100);
    run_safe_shutdown_hooks("ota");
  });
  ArduinoOTA.onProgress([this](uint progress, uint total) {
#ifdef USE_STATUS_LED
    if (global_status_led != nullptr) {
      global_status_led->loop_();
    }
#endif
    if (this->at_ota_progress_message_++ % 8 != 0)
      return; // only print every 8th message
    float percentage = float(progress) * 100 / float(total);
    ESP_LOGD(TAG, "OTA in progress: %0.1f%%", percentage);
  });
  ArduinoOTA.onError([this](ota_error_t error) {
    ESP_LOGE(TAG, "Error[%u]: ", error);
    switch (error) {
      case OTA_AUTH_ERROR: {
        ESP_LOGE(TAG, "  Auth Failed");
        break;
      }
      case OTA_BEGIN_ERROR: {
        ESP_LOGE(TAG, "  Begin Failed");
        break;
      }
      case OTA_CONNECT_ERROR: {
        ESP_LOGE(TAG, "  Connect Failed");
        break;
      }
      case OTA_RECEIVE_ERROR: {
        ESP_LOGE(TAG, "  Receive Failed");
        break;
      }
      case OTA_END_ERROR: {
        ESP_LOGE(TAG, "  End Failed");
        break;
      }
      default:ESP_LOGE(TAG, "  Unknown Error");
    }
    this->ota_triggered_ = false;
    this->status_clear_warning();
    this->status_momentary_error("onerror", 5000);
  });
  ArduinoOTA.begin();

#ifdef ARDUINO_ARCH_ESP32
  add_shutdown_hook([](const char *cause) {
    if (strcmp(cause, "ota") != 0)
      ArduinoOTA.end();
  });
#endif

  if (this->has_safe_mode_) {
    add_safe_shutdown_hook([this](const char *cause) {
      if (strcmp(cause, "ota") != 0)
        this->clean_rtc();
    });

    if (this->safe_mode_rtc_value_ > 1) {
      ESP_LOGW(TAG, "Last Boot was an unhandled reset, will proceed to safe mode in %d restarts",
               this->safe_mode_num_attempts_ - this->safe_mode_rtc_value_);
    }
  }
}

void OTAComponent::loop() {
  do {
    ArduinoOTA.handle();
#ifdef USE_STATUS_LED
    if (global_status_led != nullptr) {
      global_status_led->loop_();
    }
#endif
    yield();
  } while (this->ota_triggered_);

  if (this->has_safe_mode_ && (millis() - this->safe_mode_start_time_) > this->safe_mode_enable_time_) {
    this->has_safe_mode_ = false;
    // successful boot, reset counter
    ESP_LOGI(TAG, "Boot seems successful, resetting boot loop counter.");
    this->write_rtc_(0);
  }
}

OTAComponent::OTAComponent(uint16_t port, std::string hostname)
    : port_(port), hostname_(std::move(hostname)), auth_type_(OPEN), server_(nullptr) {

}

void OTAComponent::set_auth_open() {
  this->auth_type_ = OPEN;
}
void OTAComponent::set_auth_plaintext_password(const std::string &password) {
  this->auth_type_ = PLAINTEXT;
  this->password_ = password;
}
void OTAComponent::set_auth_password_hash(const std::string &hash) {
  this->auth_type_ = HASH;
  this->password_ = hash;
}
float OTAComponent::get_setup_priority() const {
  return setup_priority::MQTT_CLIENT + 1.0f;
}
uint16_t OTAComponent::get_port() const {
  return this->port_;
}
void OTAComponent::set_port(uint16_t port) {
  this->port_ = port;
}
const std::string &OTAComponent::get_hostname() const {
  return this->hostname_;
}
void OTAComponent::set_hostname(const std::string &hostname) {
  this->hostname_ = sanitize_hostname(hostname);
}
void OTAComponent::start_safe_mode(uint8_t num_attempts, uint32_t enable_time) {
  this->has_safe_mode_ = true;
  this->safe_mode_start_time_ = millis();
  this->safe_mode_enable_time_ = enable_time;
  this->safe_mode_num_attempts_ = num_attempts;
  this->safe_mode_rtc_value_ = this->read_rtc_();

  ESP_LOGCONFIG(TAG, "There have been %u suspected unsuccessful boot attempts.", this->safe_mode_rtc_value_);

  if (this->safe_mode_rtc_value_ >= num_attempts) {
    this->clean_rtc();

    ESP_LOGE(TAG, "Boot loop detected. Proceeding to safe mode.");
    assert(global_wifi_component != nullptr);

#ifdef USE_STATUS_LED
    if (global_status_led != nullptr) {
      global_status_led->setup_();
    }
#endif
    global_state = STATUS_LED_ERROR;
    global_wifi_component->setup_();
    while (!global_wifi_component->can_proceed()) {
      yield();
      global_wifi_component->loop_();
#ifdef USE_STATUS_LED
      if (global_status_led != nullptr) {
        global_status_led->loop_();
      }
#endif
    }
    this->setup_();

    ESP_LOGI(TAG, "Waiting for OTA attempt.");
    uint32_t begin = millis();
    while ((millis() - begin) < enable_time) {
      this->loop_();
      global_wifi_component->loop_();
      yield();
    }
    ESP_LOGE(TAG, "No OTA attempt made, restarting.");
    reboot("ota-safe-mode");
  } else {
    // increment counter
    this->write_rtc_(uint8_t(this->safe_mode_rtc_value_ + 1));
  }
}
void OTAComponent::write_rtc_(uint8_t val) {
#ifdef ARDUINO_ARCH_ESP8266
  uint32_t data = val;
  ESP.rtcUserMemoryWrite(0, &data, sizeof(data));
#endif
#ifdef ARDUINO_ARCH_ESP32
  global_preferences.put_uint8(PREF_TAG, PREF_SAFE_MODE_COUNTER_KEY, static_cast<uint8_t>(val));
#endif
}
uint8_t OTAComponent::read_rtc_() {
#ifdef ARDUINO_ARCH_ESP8266
  uint32_t rtc_data;
  ESP.rtcUserMemoryRead(0, &rtc_data, sizeof(rtc_data));
  if (rtc_data > 255) // num attempts 255 at max
    return 0;
  return uint8_t(rtc_data);
#endif
#ifdef ARDUINO_ARCH_ESP32
  return global_preferences.get_uint8(PREF_TAG, PREF_SAFE_MODE_COUNTER_KEY, 0);
#endif
}
void OTAComponent::clean_rtc() {
  this->write_rtc_(0);
}

ESPHOMELIB_NAMESPACE_END

#endif //USE_OTA
