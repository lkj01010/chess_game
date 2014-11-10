#ifndef PROTO_HELPER_H
#define PROTO_HELPER_H

#include <vector>
#include <cstdint>
#include <Poco/ByteOrder.h>
#include <google/protobuf/message_lite.h>

inline std::vector<int8_t> SerializeProtoPacket(const google::protobuf::MessageLite &message,
                                         uint16_t message_type) {
  std::vector<int8_t> buffer(message.ByteSize() + 2);
  *(uint16_t*)&buffer[0] = Poco::ByteOrder::toNetwork(message_type);
  if (message.ByteSize() > 0) {
    if (!message.SerializeToArray(&buffer[0] + 2, message.ByteSize())) {
      buffer.clear();
    }
  }
  return buffer;
}

#endif /* PROTO_HELPER_H */
