#include "esphomelib/defines.h"

#ifdef USE_API

#include "esphomelib/api/subscribe_logs.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace api {

APIMessageType SubscribeLogsRequest::message_type() const {
  return APIMessageType::SUBSCRIBE_LOGS_REQUEST;
}
bool SubscribeLogsRequest::decode_varint(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 1: // LogLevel level = 1;
      this->level_ = value;
      return true;
    default:
      return false;
  }
}
uint32_t SubscribeLogsRequest::get_level() const {
  return this->level_;
}
void SubscribeLogsRequest::set_level(uint32_t level) {
  this->level_ = level;
}
} // namespace api

ESPHOMELIB_NAMESPACE_END

#endif //USE_API
