#include "esphome/application.h"

#include <utility>
#include <algorithm>

#include "esphome/log.h"
#include "esphome/espmath.h"
#include "esphome/esppreferences.h"

ESPHOME_NAMESPACE_BEGIN

#ifdef USE_MQTT
using namespace esphome::mqtt;
#endif
#ifdef USE_BINARY_SENSOR
using namespace esphome::binary_sensor;
#endif
#ifdef USE_SENSOR
using namespace esphome::sensor;
#endif
#ifdef USE_OUTPUT
using namespace esphome::output;
#endif
#ifdef USE_LIGHT
using namespace esphome::light;
#endif
#ifdef USE_FAN
using namespace esphome::fan;
#endif
#ifdef USE_SWITCH
using namespace esphome::switch_;
#endif
#ifdef USE_IO
using namespace esphome::io;
#endif
#ifdef USE_COVER
using namespace esphome::cover;
#endif
#ifdef USE_REMOTE
using namespace esphome::remote;
#endif
#ifdef USE_TIME
using namespace esphome::time;
#endif
#ifdef USE_TEXT_SENSOR
using namespace esphome::text_sensor;
#endif
#ifdef USE_STEPPER
using namespace esphome::stepper;
#endif
#ifdef USE_CLIMATE
using namespace esphome::climate;
#endif

static const char *TAG = "application";

void Application::setup() {
  ESP_LOGI(TAG, "Running through setup()...");
  ESP_LOGV(TAG, "Sorting components by setup priority...");
  std::stable_sort(this->components_.begin(), this->components_.end(), [](const Component *a, const Component *b) {
    return a->get_actual_setup_priority() > b->get_actual_setup_priority();
  });

  for (uint32_t i = 0; i < this->components_.size(); i++) {
    Component *component = this->components_[i];
    if (component->is_failed())
      continue;

    component->call_setup();
    if (component->can_proceed())
      continue;

    std::stable_sort(this->components_.begin(), this->components_.begin() + i + 1,
                     [](Component *a, Component *b) { return a->get_loop_priority() > b->get_loop_priority(); });

    do {
      uint32_t new_global_state = STATUS_LED_WARNING;
      for (uint32_t j = 0; j <= i; j++) {
        if (!this->components_[j]->is_failed()) {
          this->components_[j]->call_loop();
        }
        new_global_state |= this->components_[j]->get_component_state();
        global_state |= new_global_state;
      }
      global_state = new_global_state;
      yield();
    } while (!component->can_proceed());
  }

  this->application_state_ = COMPONENT_STATE_SETUP;

  ESP_LOGI(TAG, "setup() finished successfully!");
  this->dump_config();
}

void Application::dump_config() {
  if (this->compilation_time_.empty()) {
    ESP_LOGI(TAG, "esphome-core version " ESPHOME_VERSION);
  } else {
    ESP_LOGI(TAG, "esphome-core version " ESPHOME_VERSION " compiled on %s", this->compilation_time_.c_str());
  }

  for (auto component : this->components_) {
    component->dump_config();
  }
}
void Application::schedule_dump_config() { this->dump_config_scheduled_ = true; }

void HOT Application::loop() {
  bool first_loop = this->application_state_ == COMPONENT_STATE_SETUP;
  if (first_loop) {
    ESP_LOGI(TAG, "Running through first loop()");
    this->application_state_ = COMPONENT_STATE_LOOP;
  }

  uint32_t new_global_state = 0;
  for (Component *component : this->components_) {
    if (!component->is_failed()) {
      component->call_loop();
    }
    new_global_state |= component->get_component_state();
    global_state |= new_global_state;
    feed_wdt();
  }
  global_state = new_global_state;

  const uint32_t now = millis();
  if (HighFrequencyLoopRequester::is_high_frequency()) {
    yield();
  } else {
    uint32_t delay_time = this->loop_interval_;
    if (now - this->last_loop_ < this->loop_interval_)
      delay_time = this->loop_interval_ - (now - this->last_loop_);
    delay(delay_time);
  }
  this->last_loop_ = now;

  if (first_loop) {
    ESP_LOGI(TAG, "First loop finished successfully!");
  }

  if (this->dump_config_scheduled_) {
    this->dump_config();
    this->dump_config_scheduled_ = false;
  }
}

WiFiComponent *Application::init_wifi(const std::string &ssid, const std::string &password) {
  WiFiComponent *wifi = this->init_wifi();
  WiFiAP ap;
  ap.set_ssid(ssid);
  ap.set_password(password);
  wifi->add_sta(ap);
  return wifi;
}

#ifdef USE_ETHERNET
EthernetComponent *Application::init_ethernet() {
  auto *eth = new EthernetComponent();
  return this->register_component(eth);
}
#endif

void Application::set_name(const std::string &name) {
  this->name_ = to_lowercase_underscore(name);
  global_preferences.begin(name);
}

void Application::set_compilation_datetime(const char *str) { this->compilation_time_ = str; }
const std::string &Application::get_compilation_time() const { return this->compilation_time_; }

#ifdef USE_MQTT
MQTTClientComponent *Application::init_mqtt(const std::string &address, uint16_t port, const std::string &username,
                                            const std::string &password) {
  MQTTClientComponent *component = new MQTTClientComponent(
      MQTTCredentials{
          .address = address,
          .port = port,
          .username = username,
          .password = password,
      },
      this->get_name());
  this->mqtt_client_ = component;

  return this->register_component(component);
}
#endif

#ifdef USE_MQTT
MQTTClientComponent *Application::init_mqtt(const std::string &address, const std::string &username,
                                            const std::string &password) {
  return this->init_mqtt(address, 1883, username, password);
}
#endif

LogComponent *Application::init_log(uint32_t baud_rate, size_t tx_buffer_size, UARTSelection uart) {
  auto *log = new LogComponent(baud_rate, tx_buffer_size, uart);
  log->pre_setup();
  return this->register_component(log);
}

#ifdef USE_OUTPUT
PowerSupplyComponent *Application::make_power_supply(const GPIOOutputPin &pin, uint32_t enable_time,
                                                     uint32_t keep_on_time) {
  auto *atx = new PowerSupplyComponent(pin.copy(), enable_time, keep_on_time);
  return this->register_component(atx);
}
#endif

#ifdef USE_BINARY_SENSOR
void Application::register_binary_sensor(binary_sensor::BinarySensor *binary_sensor) {
  for (auto *controller : this->controllers_)
    controller->register_binary_sensor(binary_sensor);
#ifdef USE_MQTT_BINARY_SENSOR
  if (this->mqtt_client_ != nullptr) {
    binary_sensor->set_mqtt(this->register_component(new MQTTBinarySensorComponent(binary_sensor)));
  }
#endif
}
#endif

#ifdef USE_GPIO_BINARY_SENSOR
GPIOBinarySensorComponent *Application::make_gpio_binary_sensor(const std::string &friendly_name,
                                                                const GPIOInputPin &pin,
                                                                const std::string &device_class) {
  auto *comp = this->register_component(new GPIOBinarySensorComponent(friendly_name, pin.copy()));
  comp->set_device_class(device_class);
  this->register_binary_sensor(comp);
  return comp;
}
#endif

#ifdef USE_DHT_SENSOR
DHTComponent *Application::make_dht_sensor(const std::string &temperature_friendly_name,
                                           const std::string &humidity_friendly_name, const GPIOOutputPin &pin,
                                           uint32_t check_interval) {
  auto *dht = new DHTComponent(temperature_friendly_name, humidity_friendly_name, pin.copy(), check_interval);
  this->register_component(dht);
  this->register_sensor(dht->get_temperature_sensor());
  this->register_sensor(dht->get_humidity_sensor());
  return dht;
}
#endif

#ifdef USE_SENSOR
void Application::register_sensor(sensor::Sensor *sensor) {
  for (auto *controller : this->controllers_)
    controller->register_sensor(sensor);
#ifdef USE_MQTT_SENSOR
  if (this->mqtt_client_ != nullptr) {
    sensor->set_mqtt(this->register_component(new MQTTSensorComponent(sensor)));
  }
#endif
}
#endif

#ifdef USE_LEDC_OUTPUT
LEDCOutputComponent *Application::make_ledc_output(uint8_t pin, float frequency, uint8_t bit_depth) {
  auto *ledc = new LEDCOutputComponent(pin, frequency, bit_depth);
  return this->register_component(ledc);
}
#endif

#ifdef USE_PCA9685_OUTPUT
PCA9685OutputComponent *Application::make_pca9685_component(float frequency) {
  auto *pca9685 = new PCA9685OutputComponent(this->i2c_, frequency);
  return this->register_component(pca9685);
}
#endif

#ifdef USE_MY9231_OUTPUT
MY9231OutputComponent *Application::make_my9231_component(const GPIOOutputPin &pin_di, const GPIOOutputPin &pin_dcki) {
  auto *my9231 = new MY9231OutputComponent(pin_di.copy(), pin_dcki.copy());
  return this->register_component(my9231);
}
#endif

#ifdef USE_SM16716_OUTPUT
SM16716OutputComponent *Application::make_sm16716_component(const GPIOOutputPin &pin_data,
                                                            const GPIOOutputPin &pin_clock) {
  auto *sm16716 = new SM16716OutputComponent(pin_data.copy(), pin_clock.copy());
  return this->register_component(sm16716);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_rgb_light(const std::string &friendly_name, FloatOutput *red,
                                                   FloatOutput *green, FloatOutput *blue) {
  auto *out = new RGBLightOutput(red, green, blue);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_rgbw_light(const std::string &friendly_name, output::FloatOutput *red,
                                                    output::FloatOutput *green, output::FloatOutput *blue,
                                                    output::FloatOutput *white) {
  auto *out = new RGBWLightOutput(red, green, blue, white);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_rgbww_light(const std::string &friendly_name, float cold_white_mireds,
                                                     float warm_white_mireds, output::FloatOutput *red,
                                                     output::FloatOutput *green, output::FloatOutput *blue,
                                                     output::FloatOutput *cold_white, output::FloatOutput *warm_white) {
  auto *out = new RGBWWLightOutput(cold_white_mireds, warm_white_mireds, red, green, blue, cold_white, warm_white);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_cwww_light(const std::string &friendly_name, float cold_white_mireds,
                                                    float warm_white_mireds, output::FloatOutput *cold_white,
                                                    output::FloatOutput *warm_white) {
  auto *out = new CWWWLightOutput(cold_white_mireds, warm_white_mireds, cold_white, warm_white);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
void Application::register_light(LightState *state) {
  for (auto *controller : this->controllers_)
    controller->register_light(state);
#ifdef USE_MQTT_LIGHT
  if (this->mqtt_client_ != nullptr) {
    state->set_mqtt(this->register_component(new MQTTJSONLightComponent(state)));
  }
#endif
}
#endif

WiFiComponent *Application::get_wifi() const { return this->wifi_; }

#ifdef USE_OTA
OTAComponent *Application::init_ota() { return this->register_component(new OTAComponent()); }
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_monochromatic_light(const std::string &friendly_name,
                                                             output::FloatOutput *mono) {
  auto *out = new MonochromaticLightOutput(mono);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_binary_light(const std::string &friendly_name, output::BinaryOutput *binary) {
  auto *out = new BinaryLightOutput(binary);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_light_for_light_output(const std::string &name, light::LightOutput *output) {
  LightState *state = this->register_component(new LightState(name, output));

  this->register_light(state);
  return MakeLight{
      .output = output,
      .state = state,
  };
}
#endif

#ifdef USE_MQTT
MQTTClientComponent *Application::get_mqtt_client() const { return this->mqtt_client_; }
#endif

#ifdef USE_SWITCH
void Application::register_switch(switch_::Switch *a_switch) {
  for (auto *controller : this->controllers_)
    controller->register_switch(a_switch);
#ifdef USE_MQTT_SWITCH
  if (this->mqtt_client_ != nullptr) {
    a_switch->set_mqtt(this->register_component(new MQTTSwitchComponent(a_switch)));
  }
#endif
}
#endif

#ifdef USE_DALLAS_SENSOR
DallasComponent *Application::make_dallas_component(const GPIOOutputPin &pin, uint32_t update_interval) {
  return this->register_component(new DallasComponent(new ESPOneWire(pin.copy()), update_interval));
}
#endif

#ifdef USE_GPIO_SWITCH
switch_::GPIOSwitch *Application::make_gpio_switch(const std::string &friendly_name, const GPIOOutputPin &pin) {
  auto gpio_switch = this->register_component(new GPIOSwitch(friendly_name, pin.copy()));
  this->register_switch(gpio_switch);
  return gpio_switch;
}
#endif

const std::string &Application::get_name() const { return this->name_; }

#ifdef USE_FAN
Application::MakeFan Application::make_fan(const std::string &friendly_name) {
  MakeFan s{};
  s.state = this->register_component(new FanState(friendly_name));
  this->register_fan(s.state);
  s.output = this->register_component(new BasicFanComponent());
  s.output->set_state(s.state);
  return s;
}
#endif

#ifdef USE_GPIO_OUTPUT
output::GPIOBinaryOutputComponent *Application::make_gpio_output(const GPIOOutputPin &pin) {
  return this->register_component(new GPIOBinaryOutputComponent(pin.copy()));
}
#endif

#ifdef USE_PULSE_COUNTER_SENSOR
sensor::PulseCounterSensorComponent *Application::make_pulse_counter_sensor(const std::string &friendly_name,
                                                                            const GPIOInputPin &pin,
                                                                            uint32_t update_interval) {
  auto *pcnt = this->register_component(new PulseCounterSensorComponent(friendly_name, pin.copy(), update_interval));
  this->register_sensor(pcnt);
  return pcnt;
}
#endif

#ifdef USE_ADC_SENSOR
sensor::ADCSensorComponent *Application::make_adc_sensor(const std::string &friendly_name, uint8_t pin,
                                                         uint32_t update_interval) {
  auto *adc = this->register_component(new ADCSensorComponent(friendly_name, pin, update_interval));
  this->register_sensor(adc);
  return adc;
}
#endif

#ifdef USE_ULTRASONIC_SENSOR
sensor::UltrasonicSensorComponent *Application::make_ultrasonic_sensor(const std::string &friendly_name,
                                                                       const GPIOOutputPin &trigger_pin,
                                                                       const GPIOInputPin &echo_pin,
                                                                       uint32_t update_interval) {
  auto *ultrasonic = this->register_component(
      new UltrasonicSensorComponent(friendly_name, trigger_pin.copy(), echo_pin.copy(), update_interval));
  this->register_sensor(ultrasonic);
  return ultrasonic;
}
#endif

#ifdef USE_WIFI_SIGNAL_SENSOR
sensor::WiFiSignalSensor *Application::make_wifi_signal_sensor(const std::string &name, uint32_t update_interval) {
  auto *wifi = this->register_component(new WiFiSignalSensor(name, update_interval));
  this->register_sensor(wifi);
  return wifi;
}
#endif

#ifdef USE_ADS1115_SENSOR
ADS1115Component *Application::make_ads1115_component(uint8_t address) {
  return this->register_component(new ADS1115Component(this->i2c_, address));
}
#endif

#ifdef USE_BMP085_SENSOR
sensor::BMP085Component *Application::make_bmp085_sensor(const std::string &temperature_friendly_name,
                                                         const std::string &pressure_friendly_name,
                                                         uint32_t update_interval) {
  auto *bmp = this->register_component(
      new BMP085Component(this->i2c_, temperature_friendly_name, pressure_friendly_name, update_interval));
  this->register_sensor(bmp->get_temperature_sensor());
  this->register_sensor(bmp->get_pressure_sensor());
  return bmp;
}
#endif

#ifdef USE_HTU21D_SENSOR
sensor::HTU21DComponent *Application::make_htu21d_sensor(const std::string &temperature_friendly_name,
                                                         const std::string &humidity_friendly_name,
                                                         uint32_t update_interval) {
  auto *htu21d = this->register_component(
      new HTU21DComponent(this->i2c_, temperature_friendly_name, humidity_friendly_name, update_interval));
  this->register_sensor(htu21d->get_temperature_sensor());
  this->register_sensor(htu21d->get_humidity_sensor());
  return htu21d;
}
#endif

#ifdef USE_I2C
I2CComponent *Application::init_i2c(uint8_t sda_pin, uint8_t scl_pin, bool scan) {
  auto *i2c = this->register_component(new I2CComponent(sda_pin, scl_pin, scan));
  if (this->i2c_ == nullptr)
    this->i2c_ = i2c;
  return i2c;
}
#endif

#ifdef USE_STATUS_BINARY_SENSOR
StatusBinarySensor *Application::make_status_binary_sensor(const std::string &friendly_name) {
  auto *binary_sensor = this->register_component(new StatusBinarySensor(friendly_name));
  this->register_binary_sensor(binary_sensor);
#ifdef USE_MQTT_BINARY_SENSOR
  if (binary_sensor->get_mqtt() != nullptr) {
    auto *mqtt = binary_sensor->get_mqtt();
    mqtt->set_custom_state_topic(this->mqtt_client_->get_availability().topic);
    mqtt->disable_availability();
    mqtt->set_is_status(true);
  }
#endif
  return binary_sensor;
}
#endif

#ifdef USE_RESTART_SWITCH
switch_::RestartSwitch *Application::make_restart_switch(const std::string &friendly_name) {
  auto *a_switch = this->register_component(new RestartSwitch(friendly_name));
  this->register_switch(a_switch);
  return a_switch;
}
#endif

#ifdef USE_SHUTDOWN_SWITCH
switch_::ShutdownSwitch *Application::make_shutdown_switch(const std::string &friendly_name) {
  auto *a_switch = this->register_component(new ShutdownSwitch(friendly_name));
  this->register_switch(a_switch);
  return a_switch;
}
#endif

#ifdef USE_ESP8266_PWM_OUTPUT
ESP8266PWMOutput *Application::make_esp8266_pwm_output(GPIOOutputPin pin) {
  return this->register_component(new ESP8266PWMOutput(pin));
}
#endif

#ifdef USE_HDC1080_SENSOR
sensor::HDC1080Component *Application::make_hdc1080_sensor(const std::string &temperature_friendly_name,
                                                           const std::string &humidity_friendly_name,
                                                           uint32_t update_interval) {
  auto *hdc1080 = this->register_component(
      new HDC1080Component(this->i2c_, temperature_friendly_name, humidity_friendly_name, update_interval));
  this->register_sensor(hdc1080->get_temperature_sensor());
  this->register_sensor(hdc1080->get_humidity_sensor());
  return hdc1080;
}
#endif

#ifdef USE_DEBUG_COMPONENT
DebugComponent *Application::make_debug_component() { return this->register_component(new DebugComponent()); }
#endif

#ifdef USE_FAN
void Application::register_fan(fan::FanState *state) {
  for (auto *controller : this->controllers_)
    controller->register_fan(state);
#ifdef USE_MQTT_FAN
  if (this->mqtt_client_ != nullptr) {
    state->set_mqtt(this->register_component(new MQTTFanComponent(state)));
  }
#endif
}
#endif

#ifdef USE_WEB_SERVER
WebServer *Application::init_web_server(uint16_t port) {
  auto *web_server = new WebServer(port);
  this->register_component(web_server);
  return this->register_controller(web_server);
}
#endif

#ifdef USE_DEEP_SLEEP
DeepSleepComponent *Application::make_deep_sleep_component() {
  return this->register_component(new DeepSleepComponent());
}
#endif

WiFiComponent *Application::init_wifi() {
  auto *wifi = new WiFiComponent();
  this->wifi_ = wifi;
  return this->register_component(wifi);
}

#ifdef USE_PCF8574
PCF8574Component *Application::make_pcf8574_component(uint8_t address, bool pcf8575) {
  return this->register_component(new PCF8574Component(this->i2c_, address, pcf8575));
}
#endif

#ifdef USE_MPU6050
sensor::MPU6050Component *Application::make_mpu6050_sensor(uint8_t address, uint32_t update_interval) {
  return this->register_component(new MPU6050Component(this->i2c_, address, update_interval));
}
#endif

#ifdef USE_OUTPUT_SWITCH
switch_::OutputSwitch *Application::make_output_switch(const std::string &friendly_name, BinaryOutput *output) {
  auto *s = this->register_component(new OutputSwitch(friendly_name, output));
  this->register_switch(s);
  return s;
}
#endif

#ifdef USE_TSL2561
sensor::TSL2561Sensor *Application::make_tsl2561_sensor(const std::string &name, uint8_t address,
                                                        uint32_t update_interval) {
  auto *tsl = this->register_component(new TSL2561Sensor(this->i2c_, name, address, update_interval));
  this->register_sensor(tsl);
  return tsl;
}
#endif

#ifdef USE_BH1750
sensor::BH1750Sensor *Application::make_bh1750_sensor(const std::string &name, uint8_t address,
                                                      uint32_t update_interval) {
  auto *bh1750 = this->register_component(new BH1750Sensor(this->i2c_, name, address, update_interval));
  this->register_sensor(bh1750);
  return bh1750;
}
#endif

#ifdef USE_BME280
sensor::BME280Component *Application::make_bme280_sensor(const std::string &temperature_name,
                                                         const std::string &pressure_name,
                                                         const std::string &humidity_name, uint8_t address,
                                                         uint32_t update_interval) {
  auto *bme280 = this->register_component(
      new BME280Component(this->i2c_, temperature_name, pressure_name, humidity_name, address, update_interval));
  this->register_sensor(bme280->get_temperature_sensor());
  this->register_sensor(bme280->get_pressure_sensor());
  this->register_sensor(bme280->get_humidity_sensor());
  return bme280;
}
#endif

#ifdef USE_BMP280
sensor::BMP280Component *Application::make_bmp280_sensor(const std::string &temperature_name,
                                                         const std::string &pressure_name, uint8_t address,
                                                         uint32_t update_interval) {
  auto *bmp280 = this->register_component(
      new BMP280Component(this->i2c_, temperature_name, pressure_name, address, update_interval));
  this->register_sensor(bmp280->get_temperature_sensor());
  this->register_sensor(bmp280->get_pressure_sensor());
  return bmp280;
}
#endif

#ifdef USE_BME680
sensor::BME680Component *Application::make_bme680_sensor(const std::string &temperature_name,
                                                         const std::string &pressure_name,
                                                         const std::string &humidity_name,
                                                         const std::string &gas_resistance_name, uint8_t address,
                                                         uint32_t update_interval) {
  auto *bme680 = this->register_component(new BME680Component(
      this->i2c_, temperature_name, pressure_name, humidity_name, gas_resistance_name, address, update_interval));
  this->register_sensor(bme680->get_temperature_sensor());
  this->register_sensor(bme680->get_pressure_sensor());
  this->register_sensor(bme680->get_humidity_sensor());
  this->register_sensor(bme680->get_gas_resistance_sensor());
  return bme680;
}
#endif

#ifdef USE_SHT3XD
sensor::SHT3XDComponent *Application::make_sht3xd_sensor(const std::string &temperature_name,
                                                         const std::string &humidity_name, uint8_t address,
                                                         uint32_t update_interval) {
  auto *sht3xd = this->register_component(
      new SHT3XDComponent(this->i2c_, temperature_name, humidity_name, address, update_interval));
  this->register_sensor(sht3xd->get_temperature_sensor());
  this->register_sensor(sht3xd->get_humidity_sensor());
  return sht3xd;
}
#endif

#ifdef USE_ESP32_TOUCH_BINARY_SENSOR
ESP32TouchComponent *Application::make_esp32_touch_component() {
  return this->register_component(new ESP32TouchComponent());
}
#endif

#ifdef USE_ESP32_BLE_TRACKER
ESP32BLETracker *Application::make_esp32_ble_tracker() { return this->register_component(new ESP32BLETracker()); }
#endif

#ifdef USE_FAST_LED_LIGHT
Application::MakeFastLEDLight Application::make_fast_led_light(const std::string &name) {
  auto *fast_led = this->register_component(new FastLEDLightOutputComponent());
  auto make = this->make_light_for_light_output(name, fast_led);

  return MakeFastLEDLight{
      .fast_led = fast_led,
      .state = make.state,
  };
}
#endif

#ifdef USE_DHT12_SENSOR
sensor::DHT12Component *Application::make_dht12_sensor(const std::string &temperature_name,
                                                       const std::string &humidity_name, uint32_t update_interval) {
  auto *dht12 =
      this->register_component(new DHT12Component(this->i2c_, temperature_name, humidity_name, update_interval));
  this->register_sensor(dht12->get_temperature_sensor());
  this->register_sensor(dht12->get_humidity_sensor());
  return dht12;
}
#endif

#ifdef USE_ROTARY_ENCODER_SENSOR
sensor::RotaryEncoderSensor *Application::make_rotary_encoder_sensor(const std::string &name, const GPIOInputPin &pin_a,
                                                                     const GPIOInputPin &pin_b) {
  auto *encoder = this->register_component(new RotaryEncoderSensor(name, pin_a.copy(), pin_b.copy()));
  this->register_sensor(encoder);
  return encoder;
}
#endif

StartupTrigger *Application::make_startup_trigger() { return this->register_component(new StartupTrigger()); }

ShutdownTrigger *Application::make_shutdown_trigger() { return new ShutdownTrigger(); }

#ifdef USE_TEMPLATE_SENSOR
sensor::TemplateSensor *Application::make_template_sensor(const std::string &name, uint32_t update_interval) {
  auto *a_template = this->register_component(new TemplateSensor(name, update_interval));
  this->register_sensor(a_template);
  return a_template;
}
#endif

#ifdef USE_MAX31855_SENSOR
sensor::MAX31855Sensor *Application::make_max31855_sensor(const std::string &name, SPIComponent *spi_bus,
                                                          const GPIOOutputPin &cs, uint32_t update_interval) {
  auto *sensor = this->register_component(new MAX31855Sensor(name, spi_bus, cs.copy(), update_interval));
  this->register_sensor(sensor);
  return sensor;
}
#endif

#ifdef USE_MAX6675_SENSOR
sensor::MAX6675Sensor *Application::make_max6675_sensor(const std::string &name, SPIComponent *spi_bus,
                                                        const GPIOOutputPin &cs, uint32_t update_interval) {
  auto *sensor = this->register_component(new MAX6675Sensor(name, spi_bus, cs.copy(), update_interval));
  this->register_sensor(sensor);
  return sensor;
}
#endif

#ifdef USE_TEMPLATE_BINARY_SENSOR
binary_sensor::TemplateBinarySensor *Application::make_template_binary_sensor(const std::string &name) {
  auto *a_template = this->register_component(new TemplateBinarySensor(name));
  this->register_binary_sensor(a_template);
  return a_template;
}
#endif

#ifdef USE_TEMPLATE_SWITCH
switch_::TemplateSwitch *Application::make_template_switch(const std::string &name) {
  auto *a_template = this->register_component(new TemplateSwitch(name));
  this->register_switch(a_template);
  return a_template;
}
#endif

#ifdef USE_COVER
void Application::register_cover(cover::Cover *cover) {
  for (auto *controller : this->controllers_)
    controller->register_cover(cover);
#ifdef USE_MQTT_COVER
  if (this->mqtt_client_ != nullptr) {
    cover->set_mqtt(this->register_component(new MQTTCoverComponent(cover)));
  }
#endif
}
#endif

#ifdef USE_REMOTE_TRANSMITTER
remote::RemoteTransmitterComponent *Application::make_remote_transmitter_component(const GPIOOutputPin &output) {
  return this->register_component(new remote::RemoteTransmitterComponent(output.copy()));
}
#endif

#ifdef USE_REMOTE_RECEIVER
remote::RemoteReceiverComponent *Application::make_remote_receiver_component(const GPIOInputPin &output) {
  return this->register_component(new remote::RemoteReceiverComponent(output.copy()));
}
#endif

#ifdef USE_DUTY_CYCLE_SENSOR
sensor::DutyCycleSensor *Application::make_duty_cycle_sensor(const std::string &name, const GPIOInputPin &pin,
                                                             uint32_t update_interval) {
  auto *duty = this->register_component(new DutyCycleSensor(name, pin.copy(), update_interval));
  this->register_sensor(duty);
  return duty;
}
#endif

#ifdef USE_ESP32_HALL_SENSOR
sensor::ESP32HallSensor *Application::make_esp32_hall_sensor(const std::string &name, uint32_t update_interval) {
  auto *hall = this->register_component(new ESP32HallSensor(name, update_interval));
  this->register_sensor(hall);
  return hall;
}
#endif

#ifdef USE_ESP32_BLE_BEACON
ESP32BLEBeacon *Application::make_esp32_ble_beacon(const std::array<uint8_t, 16> &uuid) {
  return this->register_component(new ESP32BLEBeacon(uuid));
}
#endif

#ifdef USE_STATUS_LED
StatusLEDComponent *Application::make_status_led(const GPIOOutputPin &pin) {
  return this->register_component(new StatusLEDComponent(pin.copy()));
}
#endif

#ifdef USE_MHZ19
sensor::MHZ19Component *Application::make_mhz19_sensor(UARTComponent *parent, const std::string &co2_name,
                                                       uint32_t update_interval) {
  auto *mhz19 = this->register_component(new MHZ19Component(parent, co2_name, update_interval));
  this->register_sensor(mhz19->get_co2_sensor());
  return mhz19;
}
#endif

#ifdef USE_UART
UARTComponent *Application::init_uart(uint32_t baud_rate) {
  return this->register_component(new UARTComponent(baud_rate));
}
#endif

#ifdef USE_SPI
SPIComponent *Application::init_spi(const GPIOOutputPin &clk, const GPIOInputPin &miso, const GPIOOutputPin &mosi) {
  return this->register_component(new SPIComponent(clk.copy(), miso.copy(), mosi.copy()));
}
SPIComponent *Application::init_spi(const GPIOOutputPin &clk) {
  return this->register_component(new SPIComponent(clk.copy(), nullptr, nullptr));
}
#endif

#ifdef USE_PN532
PN532Component *Application::make_pn532_component(SPIComponent *parent, const GPIOOutputPin &cs,
                                                  uint32_t update_interval) {
  return this->register_component(new PN532Component(parent, cs.copy(), update_interval));
}
#endif

#ifdef USE_UART_SWITCH
switch_::UARTSwitch *Application::make_uart_switch(UARTComponent *parent, const std::string &name,
                                                   const std::vector<uint8_t> &data) {
  auto *uart = new UARTSwitch(parent, name, data);
  this->register_switch(uart);
  return uart;
}
#endif

#ifdef USE_UPTIME_SENSOR
sensor::UptimeSensor *Application::make_uptime_sensor(const std::string &name, uint32_t update_interval) {
  auto *uptime = this->register_component(new UptimeSensor(name, update_interval));
  this->register_sensor(uptime);
  return uptime;
}
#endif

#ifdef USE_INA219
sensor::INA219Component *Application::make_ina219(float shunt_resistance_ohm, float max_current_a, float max_voltage_v,
                                                  uint8_t address, uint32_t update_interval) {
  return this->register_component(
      new INA219Component(this->i2c_, shunt_resistance_ohm, max_current_a, max_voltage_v, address, update_interval));
}
#endif

#ifdef USE_INA3221
sensor::INA3221Component *Application::make_ina3221(uint8_t address, uint32_t update_interval) {
  return this->register_component(new INA3221Component(this->i2c_, address, update_interval));
}
#endif

#ifdef USE_HMC5883L
sensor::HMC5883LComponent *Application::make_hmc5883l(uint32_t update_interval) {
  return this->register_component(new HMC5883LComponent(this->i2c_, update_interval));
}
#endif

#ifdef USE_HX711
sensor::HX711Sensor *Application::make_hx711_sensor(const std::string &name, const GPIOInputPin &dout,
                                                    const GPIOOutputPin &sck, uint32_t update_interval) {
  auto *hx711 = this->register_component(new HX711Sensor(name, dout.copy(), sck.copy(), update_interval));
  this->register_sensor(hx711);
  return hx711;
}
#endif

#ifdef USE_MS5611
sensor::MS5611Component *Application::make_ms5611_sensor(const std::string &temperature_name,
                                                         const std::string &pressure_name, uint32_t update_interval) {
  auto *ms5611 =
      this->register_component(new MS5611Component(this->i2c_, temperature_name, pressure_name, update_interval));
  this->register_sensor(ms5611->get_temperature_sensor());
  this->register_sensor(ms5611->get_pressure_sensor());
  return ms5611;
}
#endif

#ifdef USE_RDM6300
binary_sensor::RDM6300Component *Application::make_rdm6300_component(UARTComponent *parent) {
  return this->register_component(new RDM6300Component(parent));
}
#endif
bool Application::is_fully_setup() const {
  return this->application_state_ == COMPONENT_STATE_SETUP || this->application_state_ == COMPONENT_STATE_LOOP;
}

#ifdef USE_TCS34725
sensor::TCS34725Component *Application::make_tcs34725(uint32_t update_interval) {
  return this->register_component(new TCS34725Component(this->i2c_, update_interval));
}
#endif

#ifdef USE_MAX7219
display::MAX7219Component *Application::make_max7219(SPIComponent *parent, const GPIOOutputPin &cs,
                                                     uint32_t update_interval) {
  return this->register_component(new display::MAX7219Component(parent, cs.copy(), update_interval));
}
#endif

#ifdef USE_LCD_DISPLAY_PCF8574
display::PCF8574LCDDisplay *Application::make_pcf8574_lcd_display(uint8_t columns, uint8_t rows, uint8_t address,
                                                                  uint32_t update_interval) {
  return this->register_component(new display::PCF8574LCDDisplay(this->i2c_, columns, rows, address, update_interval));
}
#endif

#ifdef USE_SSD1306
#ifdef USE_SPI
display::SPISSD1306 *Application::make_spi_ssd1306(SPIComponent *parent, const GPIOOutputPin &cs,
                                                   const GPIOOutputPin &dc, uint32_t update_interval) {
  return this->register_component(new display::SPISSD1306(parent, cs.copy(), dc.copy(), update_interval));
}
#endif

#ifdef USE_I2C
display::I2CSSD1306 *Application::make_i2c_ssd1306(uint32_t update_interval) {
  return this->register_component(new display::I2CSSD1306(this->i2c_, update_interval));
}
#endif
#endif

#ifdef USE_WAVESHARE_EPAPER
display::WaveshareEPaperTypeA *Application::make_waveshare_epaper_type_a(SPIComponent *parent, const GPIOOutputPin &cs,
                                                                         const GPIOOutputPin &dc_pin,
                                                                         display::WaveshareEPaperTypeAModel model,
                                                                         uint32_t update_interval) {
  return this->register_component(
      new display::WaveshareEPaperTypeA(parent, cs.copy(), dc_pin.copy(), model, update_interval));
}

display::WaveshareEPaper *Application::make_waveshare_epaper_type_b(SPIComponent *parent, const GPIOOutputPin &cs,
                                                                    const GPIOOutputPin &dc_pin,
                                                                    display::WaveshareEPaperTypeBModel model,
                                                                    uint32_t update_interval) {
  display::WaveshareEPaper *ret = nullptr;
  switch (model) {
    case display::WAVESHARE_EPAPER_2_7_IN:
      ret = new display::WaveshareEPaper2P7In(parent, cs.copy(), dc_pin.copy(), update_interval);
      break;
    case display::WAVESHARE_EPAPER_4_2_IN:
      ret = new display::WaveshareEPaper4P2In(parent, cs.copy(), dc_pin.copy(), update_interval);
      break;
    case display::WAVESHARE_EPAPER_7_5_IN:
      ret = new display::WaveshareEPaper7P5In(parent, cs.copy(), dc_pin.copy(), update_interval);
      break;
  }
  return this->register_component(ret);
}
#endif

#ifdef USE_DISPLAY
display::Font *Application::make_font(std::vector<display::Glyph> &&glyphs, int baseline, int bottom) {
  return new display::Font(std::move(glyphs), baseline, bottom);
}
display::Image *Application::make_image(const uint8_t *data_start, int width, int height) {
  return new display::Image(data_start, width, height);
}
#endif

#ifdef USE_LCD_DISPLAY
display::GPIOLCDDisplay *Application::make_gpio_lcd_display(uint8_t columns, uint8_t rows, uint32_t update_interval) {
  return this->register_component(new display::GPIOLCDDisplay(columns, rows, update_interval));
}
#endif

#ifdef USE_SNTP_COMPONENT
SNTPComponent *Application::make_sntp_component() { return this->register_component(new SNTPComponent()); }
#endif

#ifdef USE_HOMEASSISTANT_TIME
HomeAssistantTime *Application::make_homeassistant_time_component() {
  return this->register_component(new HomeAssistantTime());
}
#endif

#ifdef USE_HLW8012
sensor::HLW8012Component *Application::make_hlw8012(const GPIOOutputPin &sel_pin, uint8_t cf_pin, uint8_t cf1_pin,
                                                    uint32_t update_interval) {
  return this->register_component(new HLW8012Component(sel_pin.copy(), cf_pin, cf1_pin, update_interval));
}
#endif

#ifdef USE_NEXTION
display::Nextion *Application::make_nextion(UARTComponent *parent, uint32_t update_interval) {
  return this->register_component(new display::Nextion(parent, update_interval));
}
#endif

#ifdef USE_TEXT_SENSOR
void Application::register_text_sensor(text_sensor::TextSensor *sensor) {
  for (auto *controller : this->controllers_)
    controller->register_text_sensor(sensor);
#ifdef USE_MQTT_TEXT_SENSOR
  if (this->mqtt_client_ != nullptr) {
    sensor->set_mqtt(this->register_component(new MQTTTextSensor(sensor)));
  }
#endif
}
#endif

#ifdef USE_MQTT_SUBSCRIBE_TEXT_SENSOR
text_sensor::MQTTSubscribeTextSensor *Application::make_mqtt_subscribe_text_sensor(const std::string &name,
                                                                                   std::string topic) {
  auto *sensor = this->register_component(new MQTTSubscribeTextSensor(name, std::move(topic)));
  this->register_text_sensor(sensor);
  return sensor;
}
#endif

#ifdef USE_HOMEASSISTANT_TEXT_SENSOR
text_sensor::HomeassistantTextSensor *Application::make_homeassistant_text_sensor(const std::string &name,
                                                                                  std::string entity_id) {
  auto *sensor = this->register_component(new HomeassistantTextSensor(name, entity_id));
  this->register_text_sensor(sensor);
  return sensor;
}
#endif

#ifdef USE_VERSION_TEXT_SENSOR
text_sensor::VersionTextSensor *Application::make_version_text_sensor(const std::string &name) {
  auto *sensor = this->register_component(new VersionTextSensor(name));
  this->register_text_sensor(sensor);
  return sensor;
}
#endif

#ifdef USE_MQTT_SUBSCRIBE_SENSOR
sensor::MQTTSubscribeSensor *Application::make_mqtt_subscribe_sensor(const std::string &name, std::string topic) {
  auto *sensor = this->register_component(new sensor::MQTTSubscribeSensor(name, std::move(topic)));
  this->register_sensor(sensor);
  return sensor;
}
#endif

#ifdef USE_HOMEASSISTANT_SENSOR
sensor::HomeassistantSensor *Application::make_homeassistant_sensor(const std::string &name, std::string entity_id) {
  auto *sensor = this->register_component(new HomeassistantSensor(name, entity_id));
  this->register_sensor(sensor);
  return sensor;
}
#endif

#ifdef USE_HOMEASSISTANT_BINARY_SENSOR
binary_sensor::HomeassistantBinarySensor *Application::make_homeassistant_binary_sensor(const std::string &name,
                                                                                        std::string entity_id) {
  auto *sensor = this->register_component(new HomeassistantBinarySensor(name, entity_id));
  this->register_binary_sensor(sensor);
  return sensor;
}
#endif

#ifdef USE_TEMPLATE_TEXT_SENSOR
text_sensor::TemplateTextSensor *Application::make_template_text_sensor(const std::string &name,
                                                                        uint32_t update_interval) {
  auto *a_template = this->register_component(new TemplateTextSensor(name, update_interval));
  this->register_text_sensor(a_template);
  return a_template;
}
#endif

#ifdef USE_CSE7766
sensor::CSE7766Component *Application::make_cse7766(UARTComponent *parent, uint32_t update_interval) {
  return this->register_component(new sensor::CSE7766Component(parent, update_interval));
}
#endif

#ifdef USE_PMSX003
sensor::PMSX003Component *Application::make_pmsx003(UARTComponent *parent, sensor::PMSX003Type type) {
  return this->register_component(new PMSX003Component(parent, type));
}
#endif

#ifdef USE_A4988
stepper::A4988 *Application::make_a4988(const GPIOOutputPin &step_pin, const GPIOOutputPin &dir_pin) {
  return this->register_component(new A4988(step_pin.copy(), dir_pin.copy()));
}
#endif

#ifdef USE_TOTAL_DAILY_ENERGY_SENSOR
sensor::TotalDailyEnergy *Application::make_total_daily_energy_sensor(const std::string &name,
                                                                      time::RealTimeClockComponent *time,
                                                                      sensor::Sensor *parent) {
  auto total = this->register_component(new TotalDailyEnergy(name, time, parent));
  this->register_sensor(total);
  return total;
}
#endif

void Application::set_loop_interval(uint32_t loop_interval) { this->loop_interval_ = loop_interval; }

void Application::register_component_(Component *comp) {
  if (comp == nullptr) {
    ESP_LOGW(TAG, "Tried to register null component!");
    return;
  }

  for (auto *c : this->components_) {
    if (comp == c) {
      ESP_LOGW(TAG, "Component already registered! (%p)", c);
      return;
    }
  }
  this->components_.push_back(comp);
}

#ifdef USE_API
api::APIServer *Application::init_api_server() {
  auto *server = new api::APIServer();
  this->register_component(server);
  this->register_controller(server);
  return server;
}
#endif

#ifdef USE_CUSTOM_BINARY_SENSOR
binary_sensor::CustomBinarySensorConstructor *Application::make_custom_binary_sensor(
    const std::function<std::vector<BinarySensor *>()> &init) {
  return this->register_component(new CustomBinarySensorConstructor(init));
}
#endif

#ifdef USE_CUSTOM_SENSOR
sensor::CustomSensorConstructor *Application::make_custom_sensor(const std::function<std::vector<Sensor *>()> &init) {
  return this->register_component(new CustomSensorConstructor(init));
}
#endif

#ifdef USE_CUSTOM_SWITCH
switch_::CustomSwitchConstructor *Application::make_custom_switch(const std::function<std::vector<Switch *>()> &init) {
  return this->register_component(new CustomSwitchConstructor(init));
}
#endif

#ifdef USE_APDS9960
sensor::APDS9960 *Application::make_apds9960(uint32_t update_interval) {
  return this->register_component(new APDS9960(this->i2c_, update_interval));
}
#endif

#ifdef USE_MPR121
binary_sensor::MPR121Component *Application::make_mpr121(uint8_t address) {
  return this->register_component(new MPR121Component(this->i2c_, address));
}
#endif

#ifdef USE_TTP229_LSF
binary_sensor::TTP229LSFComponent *Application::make_ttp229_lsf(uint8_t address) {
  return this->register_component(new TTP229LSFComponent(this->i2c_, address));
}
#endif

#ifdef USE_ULN2003
stepper::ULN2003 *Application::make_uln2003(const GPIOOutputPin &pin_a, const GPIOOutputPin &pin_b,
                                            const GPIOOutputPin &pin_c, const GPIOOutputPin &pin_d) {
  auto *uln = new stepper::ULN2003(pin_a.copy(), pin_b.copy(), pin_c.copy(), pin_d.copy());
  return this->register_component(uln);
}
#endif

#ifdef USE_LIGHT
Application::MakePartitionLight Application::make_partition_light(
    const std::string &name, const std::vector<light::AddressableSegment> &segments) {
  auto *part = App.register_component(new PartitionLightOutput(segments));
  auto make = this->make_light_for_light_output(name, part);

  return MakePartitionLight{
      .partition = part,
      .state = make.state,
  };
}
#endif

#ifdef USE_MCP23017
io::MCP23017 *Application::make_mcp23017_component(uint8_t address) {
  return this->register_component(new MCP23017(this->i2c_, address));
}
#endif

#ifdef USE_SDS011
sensor::SDS011Component *Application::make_sds011(UARTComponent *parent) {
  return this->register_component(new SDS011Component(parent));
}
#endif

#ifdef USE_CLIMATE
void Application::register_climate(climate::ClimateDevice *climate) {
  for (auto *controller : this->controllers_)
    controller->register_climate(climate);
#ifdef USE_MQTT_CLIMATE
  if (this->mqtt_client_ != nullptr) {
    climate->set_mqtt(this->register_component(new climate::MQTTClimateComponent(climate)));
  }
#endif
}
#endif

Application App;  // NOLINT

ESPHOME_NAMESPACE_END
