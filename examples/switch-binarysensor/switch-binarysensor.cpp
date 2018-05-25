//
// Created by Otto Winter on 22.01.18.
//

#include <esphomelib.h>

using namespace esphomelib;
using namespace esphomelib::switch_::ir;

static const char *TAG = "main";

void setup() {
  App.set_name("ir");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  auto *ir = App.make_ir_transmitter(32);
  App.register_switch(ir->create_transmitter("Panasonic TV On", SendData::from_panasonic(0x4004, 0x100BCBD).repeat(25)));
  App.register_switch(ir->create_transmitter("Panasonic TV Off", SendData::from_panasonic(0x4004, 0x100BCBD)));
  App.register_switch(ir->create_transmitter("Panasonic TV Mute", SendData::from_panasonic(0x4004, 0x1004C4D)));
  App.register_switch(ir->create_transmitter("Panasonic TV Volume Up", SendData::from_panasonic(0x4004, 0x1000405)));
  App.register_switch(ir->create_transmitter("Panasonic TV Volume Down", SendData::from_panasonic(0x4004, 0x1008485)));
  App.register_switch(ir->create_transmitter("Panasonic TV Program Up", SendData::from_panasonic(0x4004, 0x1002C2D)));
  App.register_switch(ir->create_transmitter("Panasonic TV Program Down", SendData::from_panasonic(0x4004, 0x100ACAD)));

  App.make_gpio_switch("Dehumidifier", 33);
  ESP_LOGV(TAG, "Humidifier created.");

  App.make_gpio_binary_sensor("Cabinet Motion", 36, "motion");

  App.setup();
}

void loop() {
  App.loop();
}
