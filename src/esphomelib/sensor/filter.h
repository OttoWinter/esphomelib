#ifndef ESPHOMELIB_SENSOR_FILTER_H
#define ESPHOMELIB_SENSOR_FILTER_H

#include "esphomelib/defines.h"

#ifdef USE_SENSOR

#include <cstdint>
#include <utility>
#include <list>
#include "esphomelib/component.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class Sensor;
class MQTTSensorComponent;

/** Apply a filter to sensor values such as moving average.
 *
 * This class is purposefully kept quite simple, since more complicated
 * filters should really be done with the filter sensor in Home Assistant.
 */
class Filter {
 public:
  /** This will be called every time the filter receives a new value.
   *
   * It can return an empty optional to indicate that the filter chain
   * should stop, otherwise the value in the filter will be passed down
   * the chain.
   *
   * @param value The new value.
   * @return An optional float, the new value that should be pushed out.
   */
  virtual optional<float> new_value(float value) = 0;

  virtual ~Filter();

  virtual void initialize(std::function<void(float)> &&output);

  void input(float value);

  /// Return the amount of time that this filter is expected to take based on the input time interval.
  virtual uint32_t expected_interval(uint32_t input);

 protected:
  friend Sensor;
  friend MQTTSensorComponent;

  std::function<void(float)> output_;
  Filter *next_{nullptr};
};

/** Simple sliding window moving average filter.
 *
 * Essentially just takes takes the average of the last window_size values and pushes them out
 * every send_every.
 */
class SlidingWindowMovingAverageFilter : public Filter {
 public:
  /** Construct a SlidingWindowMovingAverageFilter.
   *
   * @param window_size The number of values that should be averaged.
   * @param send_every After how many sensor values should a new one be pushed out.
   */
  explicit SlidingWindowMovingAverageFilter(size_t window_size, size_t send_every);

  optional<float> new_value(float value) override;

  size_t get_send_every() const;
  void set_send_every(size_t send_every);
  size_t get_window_size() const;
  void set_window_size(size_t window_size);

  uint32_t expected_interval(uint32_t input) override;

 protected:
  SlidingWindowMovingAverage value_average_;
  size_t send_every_;
  size_t send_at_;
};

/** Simple exponential moving average filter.
 *
 * Essentially just takes the average of the last few values using exponentially decaying weights.
 * Use alpha to adjust decay rate.
 */
class ExponentialMovingAverageFilter : public Filter {
 public:
  ExponentialMovingAverageFilter(float alpha, size_t send_every);

  optional<float> new_value(float value) override;

  size_t get_send_every() const;
  void set_send_every(size_t send_every);
  float get_alpha() const;
  void set_alpha(float alpha);

  uint32_t expected_interval(uint32_t input) override;

 protected:
  ExponentialMovingAverage value_average_;
  ExponentialMovingAverage accuracy_average_;
  size_t send_every_;
  size_t send_at_;
};

using lambda_filter_t = std::function<optional<float>(float)>;

/** This class allows for creation of simple template filters.
 *
 * The constructor accepts a lambda of the form float -> optional<float>.
 * It will be called with each new value in the filter chain and returns the modified
 * value that shall be passed down the filter chain. Returning an empty Optional
 * means that the value shall be discarded.
 */
class LambdaFilter : public Filter {
 public:
  explicit LambdaFilter(lambda_filter_t lambda_filter);

  optional<float> new_value(float value) override;

  const lambda_filter_t &get_lambda_filter() const;
  void set_lambda_filter(const lambda_filter_t &lambda_filter);

 protected:
  lambda_filter_t lambda_filter_;
};

/// A simple filter that adds `offset` to each value it receives.
class OffsetFilter : public Filter {
 public:
  explicit OffsetFilter(float offset);

  optional<float> new_value(float value) override;

 protected:
  float offset_;
};

/// A simple filter that multiplies to each value it receives by `multiplier`.
class MultiplyFilter : public Filter {
 public:
  explicit MultiplyFilter(float multiplier);

  optional<float> new_value(float value) override;

 protected:
  float multiplier_;
};

/// A simple filter that only forwards the filter chain if it doesn't receive `value_to_filter_out`.
class FilterOutValueFilter : public Filter {
 public:
  explicit FilterOutValueFilter(float values_to_filter_out);

  optional<float> new_value(float value) override;

 protected:
  float value_to_filter_out_;
};

/// A simple filter that only forwards the filter chain if it doesn't receive `nan`.
class FilterOutNANFilter : public Filter {
 public:
  optional<float> new_value(float value) override;
};

class ThrottleFilter : public Filter {
 public:
  explicit ThrottleFilter(uint32_t min_time_between_inputs);

  optional<float> new_value(float value) override;

 protected:
  uint32_t last_input_{0};
  uint32_t min_time_between_inputs_;
};

class DebounceFilter : public Filter, public Component {
 public:
  explicit DebounceFilter(uint32_t time_period);

  optional<float> new_value(float value) override;

 protected:
  uint32_t time_period_;
};

class HeartbeatFilter : public Filter, public Component {
 public:
  explicit HeartbeatFilter(uint32_t time_period);

  void setup() override;

  optional<float> new_value(float value) override;

  uint32_t expected_interval(uint32_t input) override;

 protected:
  uint32_t time_period_;
  float last_input_;
};

class DeltaFilter : public Filter {
 public:
  explicit DeltaFilter(float min_delta);

  optional<float> new_value(float value) override;

 protected:
  float min_delta_;
  float last_value_{NAN};
};

class OrFilter : public Filter {
 public:
  explicit OrFilter(std::list<Filter *> filters);

  ~OrFilter() override;
  void initialize(std::function<void(float)> &&output) override;
  uint32_t expected_interval(uint32_t input) override;

  optional<float> new_value(float value) override;

 protected:
  std::list<Filter *> filters_;
};

class UniqueFilter : public Filter {
 public:
  optional<float> new_value(float value) override;

 protected:
  float last_value_{NAN};
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SENSOR

#endif //ESPHOMELIB_SENSOR_FILTER_H
