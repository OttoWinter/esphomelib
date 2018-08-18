//
//  remote_protocol.cpp
//  esphomelib
//
//  Created by Otto Winter on 05.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_protocol.h"
#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/helpers.h"

#ifdef ARDUINO_ARCH_ESP32
  #include <driver/rmt.h>
  #include <soc/rmt_struct.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
  #include "FunctionalInterrupt.h"
#endif

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.base";

RemoteControlComponentBase::RemoteControlComponentBase(GPIOPin *pin)
    : pin_(pin) {
#ifdef ARDUINO_ARCH_ESP32
  this->channel_ = select_next_rmt_channel();
#endif
}
#ifdef ARDUINO_ARCH_ESP32
uint32_t RemoteControlComponentBase::from_microseconds(uint32_t us) {
  const uint32_t ticks_per_ten_us = 80000000u / this->clock_divider_ / 100000u;
  return us * ticks_per_ten_us / 10;
}
uint32_t RemoteControlComponentBase::to_microseconds(uint32_t ticks) {
  const uint32_t ticks_per_ten_us = 80000000u / this->clock_divider_ / 100000u;
  return (ticks * 10) / ticks_per_ten_us;
}
void RemoteControlComponentBase::set_channel(rmt_channel_t channel) {
  this->channel_ = channel;
}
void RemoteControlComponentBase::set_clock_divider(uint8_t clock_divider) {
  this->clock_divider_ = clock_divider;
}
#endif


#ifdef USE_REMOTE_RECEIVER
RemoteReceiveData::RemoteReceiveData(RemoteReceiverComponent *parent, std::vector<int32_t> *data)
    : parent_(parent), data_(data) {}

uint32_t RemoteReceiveData::lower_bound_(uint32_t length) {
  return uint32_t(100 - this->parent_->tolerance_) * length / 100U;
}
uint32_t RemoteReceiveData::upper_bound_(uint32_t length) {
  return uint32_t(100 + this->parent_->tolerance_) * length / 100U;
}
bool RemoteReceiveData::peek_mark(uint32_t length, uint32_t offset) {
  if (this->index_ + offset >= this->size())
    return false;
  int32_t value = this->peek(offset);
  const int32_t lo = this->lower_bound_(length);
  const int32_t hi = this->upper_bound_(length);
  return value >= 0 && lo <= value && value <= hi;
}
bool RemoteReceiveData::peek_space(uint32_t length, uint32_t offset) {
  if (this->index_ + offset >= this->size())
    return false;
  int32_t value = this->peek(offset);
  const int32_t lo = this->lower_bound_(length);
  const int32_t hi = this->upper_bound_(length);
  return value <= 0 && lo <= -value && -value <= hi;
}
bool RemoteReceiveData::peek_item(uint32_t mark, uint32_t space, uint32_t offset) {
  return this->peek_mark(mark, offset) && this->peek_space(space, offset + 1);
}
void RemoteReceiveData::advance(uint32_t amount) {
  this->index_ += amount;
}
bool RemoteReceiveData::expect_mark(uint32_t length) {
  if (this->peek_mark(length)) {
    this->advance();
    return true;
  }
  return false;
}
bool RemoteReceiveData::expect_space(uint32_t length) {
  if (this->peek_space(length)) {
    this->advance();
    return true;
  }
  return false;
}
bool RemoteReceiveData::expect_item(uint32_t mark, uint32_t space) {
  if (this->peek_item(mark, space)) {
    this->advance(2);
    return true;
  }
  return false;
}
void RemoteReceiveData::reset_index() {
  this->index_ = 0;
}
int32_t RemoteReceiveData::peek(uint32_t offset) {
  return (*this)[this->index_ + offset];
}
bool RemoteReceiveData::peek_space_at_least(uint32_t length, uint32_t offset) {
  if (this->index_ + offset >= this->size())
    return false;
  int32_t value = this->pos(this->index_ + offset);
  const int32_t lo = this->lower_bound_(length);
  return value <= 0 && lo <= -value;
}
int32_t RemoteReceiveData::operator[](uint32_t index) const {
  return this->pos(index);
}
int32_t RemoteReceiveData::pos(uint32_t index) const {
  return (*this->data_)[index];
}

int32_t RemoteReceiveData::size() const {
  return this->data_->size();
}

RemoteReceiverComponent::RemoteReceiverComponent(GPIOPin *pin)
    : RemoteControlComponentBase(pin) {

}

float RemoteReceiverComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

#ifdef ARDUINO_ARCH_ESP32
void RemoteReceiverComponent::setup() {
  rmt_config_t rmt{};
  ESP_LOGCONFIG(TAG, "Configuring ESP32 RMT peripheral...");
  ESP_LOGCONFIG(TAG, "    Channel: %u", this->channel_);
  rmt.channel = this->channel_;
  ESP_LOGCONFIG(TAG, "    Pin: %u", this->pin_->get_pin());
  rmt.gpio_num = gpio_num_t(this->pin_->get_pin());
  ESP_LOGCONFIG(TAG, "    Clock Divider: %u", this->clock_divider_);
  rmt.clk_div = this->clock_divider_;
  rmt.mem_block_num = 1;
  rmt.rmt_mode = RMT_MODE_RX;
  if (this->filter_us_ == 0) {
    rmt.rx_config.filter_en = false;
  } else {
    ESP_LOGCONFIG(TAG, "    Filter: %u us (%u ticks)", this->filter_us_, this->from_microseconds(this->filter_us_));
    rmt.rx_config.filter_en = true;
    rmt.rx_config.filter_ticks_thresh = this->from_microseconds(this->filter_us_);
  }
  ESP_LOGCONFIG(TAG, "    Idle threshold: %u us (%u ticks)", this->idle_us_, this->from_microseconds(this->idle_us_));
  rmt.rx_config.idle_threshold = this->from_microseconds(this->idle_us_);

  esp_err_t error = rmt_config(&rmt);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "Configuring RMT remote failed: %s", esp_err_to_name(error));
    this->mark_failed();
    return;
  }

  error = rmt_driver_install(this->channel_, this->buffer_size_, 0);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "Installing RMT driver failed: %s", esp_err_to_name(error));
    this->mark_failed();
    return;
  }
  error = rmt_get_ringbuf_handle(this->channel_, &this->ringbuf_);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "Getting RMT ringbuf handle failed: %s", esp_err_to_name(error));
    this->mark_failed();
    return;
  }
  error = rmt_rx_start(this->channel_, true);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "Starting RMT for receiving failed: %s", esp_err_to_name(error));
    this->mark_failed();
    return;
  }
}

void RemoteReceiverComponent::loop() {
  size_t len = 0;
  auto *item = (rmt_item32_t *) xRingbufferReceive(this->ringbuf_, &len, 0);
  if (item != nullptr) {
    this->decode_rmt_(item, len);
    vRingbufferReturnItem(this->ringbuf_, item);

    if (this->temp_.empty())
      return;

    RemoteReceiveData data(this, &this->temp_);
    bool found_decoder = false;
    for (auto *decoder : this->decoders_) {
      if (decoder->process_(&data))
        found_decoder = true;
    }

    if (!found_decoder) {
      for (auto *dumper : this->dumpers_)
        dumper->process_(&data);
    }
  }
}
void  RemoteReceiverComponent::decode_rmt_(rmt_item32_t *item, size_t len) {
  bool prev_level = false;
  uint32_t prev_length = 0;
  this->temp_.clear();
  int32_t multiplier = this->pin_->is_inverted() ? -1 : 1;

  ESP_LOGVV(TAG, "START:");
  for (size_t i = 0; i < len; i++) {
    if (item[i].level0) {
      ESP_LOGVV(TAG, "%u A: ON %uus (%u ticks)", i, this->to_microseconds(item[i].duration0), item[i].duration0);
    } else {
      ESP_LOGVV(TAG, "%u A: OFF %uus (%u ticks)", i, this->to_microseconds(item[i].duration0), item[i].duration0);
    }
    if (item[i].level1) {
      ESP_LOGVV(TAG, "%u B: ON %uus (%u ticks)", i, this->to_microseconds(item[i].duration1), item[i].duration1);
    } else {
      ESP_LOGVV(TAG, "%u B: OFF %uus (%u ticks)", i, this->to_microseconds(item[i].duration1), item[i].duration1);
    }
  }
  ESP_LOGVV(TAG, "\n");

  this->temp_.reserve(len / 4);
  for (size_t i = 0; i < len; i++) {
    if (item[i].duration0 == 0u) {
      // Do nothing
    } else if (bool(item[i].level0) == prev_level) {
      prev_length += item[i].duration0;
    } else {
      if (prev_length > 0) {
        if (prev_level) {
          this->temp_.push_back(this->to_microseconds(prev_length) * multiplier);
        } else {
          this->temp_.push_back(-int32_t(this->to_microseconds(prev_length)) * multiplier);
        }
      }
      prev_level = bool(item[i].level0);
      prev_length = item[i].duration0;
    }

    if (this->to_microseconds(prev_length) > this->idle_us_) {
      break;
    }

    if (item[i].duration1 == 0u) {
      // Do nothing
    } else if (bool(item[i].level1) == prev_level) {
      prev_length += item[i].duration1;
    } else {
      if (prev_length > 0) {
        if (prev_level) {
          this->temp_.push_back(this->to_microseconds(prev_length) * multiplier);
        } else {
          this->temp_.push_back(-int32_t(this->to_microseconds(prev_length)) * multiplier);
        }
      }
      prev_level = bool(item[i].level1);
      prev_length = item[i].duration1;
    }

    if (this->to_microseconds(prev_length) > this->idle_us_) {
      break;
    }
  }
  if (prev_length > 0) {
    if (prev_level) {
      this->temp_.push_back(this->to_microseconds(prev_length) * multiplier);
    } else {
      this->temp_.push_back(-int32_t(this->to_microseconds(prev_length)) * multiplier);
    }
  }
}
#endif

#ifdef ARDUINO_ARCH_ESP8266

void RemoteReceiverComponent::gpio_intr() {
  const uint32_t now = micros();
  // If the lhs is 1 (rising edge) we should write to an uneven index and vice versa
  const uint32_t next = (this->buffer_write_at_ + 1) % this->buffer_size_;
  if (uint32_t(this->pin_->digital_read()) != next % 2)
    return;
  const uint32_t last_change = this->buffer_[this->buffer_write_at_];
  if (now - last_change <= this->filter_us_)
    return;

  this->buffer_[this->buffer_write_at_ = next] = now;
}

void RemoteReceiverComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Remote Receiver...");
  this->pin_->setup();
  if (this->buffer_size_ % 2 != 0) {
    // Make sure divisible by two. This way, we know that every 0bxxx0 index is a space and every 0bxxx1 index is a mark
    this->buffer_size_++;
  }
  this->buffer_ = new uint32_t[this->buffer_size_];
  // First index is a space.
  if (this->pin_->digital_read()) {
    ESP_LOGW(TAG, "Remote Receiver Signal starts with a HIGH value. Usually this means you have to "
                  "invert the signal using 'inverted: True' !");
    this->buffer_write_at_ = this->buffer_read_at_ = 1;
    this->buffer_[1] = 0;
    this->buffer_[0] = 0;
  } else {
    this->buffer_write_at_ = this->buffer_read_at_ = 0;
    this->buffer_[0] = 0;
  }
  auto intr = std::bind(&RemoteReceiverComponent::gpio_intr, this);
  attachInterrupt(this->pin_->get_pin(), intr, CHANGE);
}

void RemoteReceiverComponent::loop() {
  // copy write at to local variables, as it's volatile
  const uint32_t write_at = this->buffer_write_at_;
  const uint32_t dist = (this->buffer_size_ + write_at - this->buffer_read_at_) % this->buffer_size_;
  // signals must at least one rising and one leading edge
  if (dist <= 1)
    return;
  const uint32_t now = micros();
  if (now - this->buffer_[write_at] < this->idle_us_)
    // The last change was fewer than the configured idle time ago.
    // TODO: Handle case when loop() is not called quickly enough to catch idle
    return;

  ESP_LOGVV(TAG, "read_at=%u write_at=%u dist=%u now=%u end=%u",
      this->buffer_read_at_, write_at, dist, now, this->buffer_[write_at]);

  // Skip first value, it's from the previous idle level
  this->buffer_read_at_ = (this->buffer_read_at_ + 1) % this->buffer_size_;
  uint32_t prev = this->buffer_read_at_;
  this->buffer_read_at_ = (this->buffer_read_at_ + 1) % this->buffer_size_;
  const uint32_t reserve_size = 1 + (this->buffer_size_ + write_at - this->buffer_read_at_) % this->buffer_size_;
  this->temp_.clear();
  this->temp_.reserve(reserve_size);
  int32_t multiplier = this->buffer_read_at_ % 2 == 0 ? 1 : -1;

  for (uint32_t i = 0; prev != write_at; i++) {
    int32_t delta = this->buffer_[this->buffer_read_at_] -  this->buffer_[prev];

    if (delta >= this->idle_us_) {
      ESP_LOGW(TAG, "Data is coming in too fast!");
      break;
    }

    ESP_LOGVV(TAG, "  i=%u buffer[%u]=%u - buffer[%u]=%u -> %d",
        i, this->buffer_read_at_, this->buffer_[this->buffer_read_at_], prev, this->buffer_[prev], multiplier * delta);
    this->temp_.push_back(multiplier * delta);
    prev = this->buffer_read_at_;
    this->buffer_read_at_ = (this->buffer_read_at_ + 1) % this->buffer_size_;
    multiplier *= -1;
  }
  this->buffer_read_at_ = (this->buffer_size_ + this->buffer_read_at_ - 1) % this->buffer_size_;
  this->temp_.push_back(this->idle_us_ * multiplier);

  RemoteReceiveData data(this, &this->temp_);
  bool found_decoder = false;
  for (auto *decoder : this->decoders_) {
    if (decoder->process_(&data))
      found_decoder = true;
  }

  if (!found_decoder) {
    for (auto *dumper : this->dumpers_)
      dumper->process_(&data);
  }
}

#endif

RemoteReceiver *RemoteReceiverComponent::add_decoder(RemoteReceiver *decoder) {
  this->decoders_.push_back(decoder);
  return decoder;
}
void RemoteReceiverComponent::add_dumper(RemoteReceiveDumper *dumper) {
  this->dumpers_.push_back(dumper);
}
void RemoteReceiverComponent::set_buffer_size(uint32_t buffer_size) {
  this->buffer_size_ = buffer_size;
}
void RemoteReceiverComponent::set_tolerance(uint8_t tolerance) {
  this->tolerance_ = tolerance;
}
void RemoteReceiverComponent::set_filter_us(uint8_t filter_us) {
  this->filter_us_ = filter_us;
}
void RemoteReceiverComponent::set_idle_us(uint32_t idle_us) {
  this->idle_us_ = idle_us;
}

RemoteReceiver::RemoteReceiver(const std::string &name)
    : BinarySensor(name) {

}

bool RemoteReceiver::process_(RemoteReceiveData *data) {
  data->reset_index();
  if (this->matches(data)) {
    this->publish_state(true);
    yield();
    this->publish_state(false);
    return true;
  }
  return false;
}

void RemoteReceiveDumper::process_(RemoteReceiveData *data) {
  data->reset_index();
  this->dump(data);
}
#endif //USE_REMOTE_RECEIVER

#ifdef USE_REMOTE_TRANSMITTER
void RemoteTransmitData::mark(uint32_t length) {
  this->data_.push_back(length);
}
void RemoteTransmitData::space(uint32_t length) {
  this->data_.push_back(-length);
}
void RemoteTransmitData::item(uint32_t mark, uint32_t space) {
  this->mark(mark);
  this->space(space);
}
void RemoteTransmitData::reserve(uint32_t len) {
  this->data_.reserve(len);
}
void RemoteTransmitData::set_data(std::vector<int32_t> data) {
  this->data_.clear();
  this->data_.reserve(data.size());
  for (int32_t i : data)
    this->data_.push_back(i);
}
void RemoteTransmitData::set_carrier_frequency(uint32_t carrier_frequency) {
  this->carrier_frequency_ = carrier_frequency;
}
uint32_t RemoteTransmitData::get_carrier_frequency() const {
  return this->carrier_frequency_;
}
const std::vector<int32_t> &RemoteTransmitData::get_data() const {
  return this->data_;
}
std::vector<int32_t>::iterator RemoteTransmitData::begin() {
  return this->data_.begin();
}
std::vector<int32_t>::iterator RemoteTransmitData::end() {
  return this->data_.end();
}
void RemoteTransmitData::reset() {
  this->data_.clear();
}

RemoteTransmitter::RemoteTransmitter(const std::string &name)
    : Switch(name) {

}

void RemoteTransmitter::turn_on() {
  this->parent_->temp_.reset();
  this->to_data(&this->parent_->temp_);
  this->parent_->send(&this->parent_->temp_, this->send_times_, this->send_wait_);
  this->publish_state(false);
}
void RemoteTransmitter::turn_off() {
  // Turning off does nothing
  this->publish_state(false);
}
void RemoteTransmitter::set_parent(RemoteTransmitterComponent *parent) {
  this->parent_ = parent;
}
void RemoteTransmitter::set_repeat(uint32_t send_times, uint32_t send_wait) {
  this->send_times_ = send_times;
  this->send_wait_ = send_wait;
}

RemoteTransmitterComponent::RemoteTransmitterComponent(GPIOPin *pin)
    : RemoteControlComponentBase(pin) {

}
float RemoteTransmitterComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
#ifdef ARDUINO_ARCH_ESP32
void RemoteTransmitterComponent::setup() {

}

void RemoteTransmitterComponent::configure_rmt() {
  rmt_config_t c{};

  ESP_LOGCONFIG(TAG, "Configuring Remote Transmitter...");
  c.rmt_mode = RMT_MODE_TX;
  c.channel = this->channel_;
  ESP_LOGCONFIG(TAG, "    Channel: %d", this->channel_);
  c.clk_div = this->clock_divider_;
  ESP_LOGCONFIG(TAG, "    Clock divider: %u", this->clock_divider_);
  c.gpio_num = gpio_num_t(this->pin_->get_pin());
  ESP_LOGCONFIG(TAG, "    GPIO Pin: %u", this->pin_->get_pin());
  c.mem_block_num = 1;
  c.tx_config.loop_en = false;

  if (this->current_carrier_frequency_ == 0 || this->carrier_duty_percent_ == 100) {
    c.tx_config.carrier_en = false;
  } else {
    c.tx_config.carrier_en = true;
    c.tx_config.carrier_freq_hz = this->current_carrier_frequency_;
    ESP_LOGCONFIG(TAG, "    Carrier Frequency: %uHz", this->current_carrier_frequency_);
    c.tx_config.carrier_duty_percent = this->carrier_duty_percent_;
    ESP_LOGCONFIG(TAG, "    Carrier Duty: %u%%", this->carrier_duty_percent_);
  }

  c.tx_config.idle_output_en = true;
  if (!this->pin_->is_inverted()) {
    ESP_LOGV(TAG, "    Carrier level: HIGH");
    c.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    ESP_LOGV(TAG, "    Idle level: LOW");
    c.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
  } else {
    c.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;
    ESP_LOGV(TAG, "    Carrier level: LOW");
    c.tx_config.idle_level = RMT_IDLE_LEVEL_HIGH;
    ESP_LOGV(TAG, "    Idle level: HIGH");
  }

  esp_err_t error = rmt_config(&c);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "Configuring RMT driver failed: %s", esp_err_to_name(error));
    this->mark_failed();
    return;
  }

  if (!this->initialized_) {
    error = rmt_driver_install(this->channel_, 0, 0);
    if (error != ESP_OK) {
      ESP_LOGE(TAG, "Error while installing RMT driver: %s", esp_err_to_name(error));
      this->mark_failed();
      return;
    }
    this->initialized_ = true;
  }
}

void RemoteTransmitterComponent::send(RemoteTransmitData *data, uint32_t send_times, uint32_t send_wait) {
  if (this->is_failed())
    return;

  if (this->current_carrier_frequency_ != data->get_carrier_frequency()) {
    this->current_carrier_frequency_ = data->get_carrier_frequency();
    this->configure_rmt();
  }

  this->rmt_temp_.clear();
  this->rmt_temp_.reserve((data->get_data().size() + 1) / 2);
  uint32_t rmt_i = 0;
  rmt_item32_t rmt_item;

  for (int32_t val : data->get_data()) {
    bool level = val >= 0;
    if (!level)
      val = -val;
    val = this->from_microseconds(val);

    do {
      int32_t item = std::min(val, 32767);
      val -= item;

      if (rmt_i % 2 == 0) {
        rmt_item.level0 = static_cast<uint32_t>(level);
        rmt_item.duration0 = static_cast<uint32_t>(item);
      } else {
        rmt_item.level1 = static_cast<uint32_t>(level);
        rmt_item.duration1 = static_cast<uint32_t>(item);
        this->rmt_temp_.push_back(rmt_item);
      }
      rmt_i++;
    } while (val != 0);
  }

  if (rmt_i % 2 == 1) {
    rmt_item.level1 = 0;
    rmt_item.duration1 = 0;
    this->rmt_temp_.push_back(rmt_item);
  }

  for (uint16_t i = 0; i < send_times; i++) {
    esp_err_t error = rmt_write_items(this->channel_, this->rmt_temp_.data(), this->rmt_temp_.size(), true);
    if (error != ESP_OK) {
      ESP_LOGW(TAG, "rmt_write_items failed: %s", esp_err_to_name(error));
      this->status_set_warning();
    } else {
      this->status_clear_warning();
    }
    if (i + 1 < send_times) {
      delay_microseconds_accurate(send_wait);
    }
  }
}
#endif //ARDUINO_ARCH_ESP32

#ifdef ARDUINO_ARCH_ESP8266
void RemoteTransmitterComponent::setup() {
  this->pin_->setup();
  this->pin_->digital_write(false);
}
void RemoteTransmitterComponent::calculate_on_off_time_(uint32_t carrier_frequency,
                                                        uint32_t *on_time_period,
                                                        uint32_t *off_time_period) {
  if (carrier_frequency == 0) {
    *on_time_period = 0;
    *off_time_period = 0;
    return;
  }
  uint32_t period = (1000000UL + carrier_frequency / 2) / carrier_frequency; // round(1000000/freq)
  period = std::max(uint32_t(1), period);
  *on_time_period = (period * this->carrier_duty_percent_) / 100;
  *off_time_period = period - *on_time_period;
}

void RemoteTransmitterComponent::send(RemoteTransmitData *data, uint32_t send_times, uint32_t send_wait) {
  for (uint32_t i = 0; i < send_times; i++) {
    uint32_t on_time, off_time;
    this->calculate_on_off_time_(data->get_carrier_frequency(), &on_time, &off_time);
    ESP_LOGD(TAG, "Sending...");

    ESP.wdtFeed();
    disable_interrupts();
    uint32_t start = micros();
    for (int32_t item : *data) {
      if (item > 0) {
        const auto length = uint32_t(item);
        this->mark_(on_time, off_time, length);
      } else {
        const auto length = uint32_t(-item);
        this->space_(length);
      }
      if (micros() - start > 5000) {
        start = micros();
        ESP.wdtFeed();
      }
    }
    enable_interrupts();

    if (i + 1 < send_times) {
      delay(send_wait / 1000UL);
      delayMicroseconds(send_wait % 1000UL);
    }
  }
}
void RemoteTransmitterComponent::mark_(uint32_t on_time, uint32_t off_time, uint32_t usec) {
  if (this->carrier_duty_percent_ == 100 || (on_time == 0 && off_time == 0)) {
    this->pin_->digital_write(true);
    delay_microseconds_accurate(usec);
    this->pin_->digital_write(false);
    return;
  }

  const uint32_t start_time = micros();
  uint32_t current_time = start_time;

  while (current_time - start_time < usec) {
    const uint32_t elapsed = current_time - start_time;
    this->pin_->digital_write(true);

    delay_microseconds_accurate(std::min(on_time, usec - elapsed));
    this->pin_->digital_write(false);
    if (elapsed + on_time >= usec)
      return;

    delay_microseconds_accurate(std::min(usec - elapsed - on_time, off_time));

    current_time = micros();
  }
}
void RemoteTransmitterComponent::space_(uint32_t usec) {
  this->pin_->digital_write(false);
  delay_microseconds_accurate(usec);
}
#endif //ARDUINO_ARCH_ESP8266

RemoteTransmitter *RemoteTransmitterComponent::add_transmitter(RemoteTransmitter *transmitter) {
  transmitter->set_parent(this);
  this->transmitters_.push_back(transmitter);
  return transmitter;
}
void RemoteTransmitterComponent::set_carrier_duty_percent(uint8_t carrier_duty_percent) {
  this->carrier_duty_percent_ = carrier_duty_percent;
}
#endif //USE_REMOTE_TRANSMITTER

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
