#ifndef ESPHOMELIB_REMOTE_SONY_H
#define ESPHOMELIB_REMOTE_SONY_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_receiver.h"
#include "esphomelib/remote/remote_transmitter.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class SonyTransmitter : public RemoteTransmitter {
 public:
  SonyTransmitter(const std::string &name, uint32_t data, uint8_t nbits);

  void to_data(RemoteTransmitData *data) override;

 protected:
  uint32_t data_;
  uint8_t nbits_;
};

void encode_sony(RemoteTransmitData *data, uint32_t data_, uint8_t nbits);
#endif

#ifdef USE_REMOTE_RECEIVER
SonyDecodeData decode_sony(RemoteReceiveData *data);

class SonyReceiver : public RemoteReceiver {
 public:
  SonyReceiver(const std::string &name, uint32_t data, uint8_t nbits);

 protected:
  bool matches(RemoteReceiveData *data) override;

 protected:
  uint32_t data_;
  uint8_t nbits_;
};

class SonyDumper : public RemoteReceiveDumper {
 public:
  bool dump(RemoteReceiveData *data) override;
};
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_REMOTE_SONY_H
