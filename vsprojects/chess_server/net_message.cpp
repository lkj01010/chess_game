#include <cassert>
#include "net_message.h"
#include <Poco/ByteOrder.h>


std::vector<int8_t> SerializeProtoPacket(const google::protobuf::MessageLite &message,
                                         uint16_t message_type) {
  std::vector<int8_t> buffer(message.ByteSize() + PROTO_HEADER_SIZE);
  *(uint16_t*)&buffer[0] = Poco::ByteOrder::toNetwork(message_type);
  if (message.ByteSize() > 0) {
    if (!message.SerializeToArray(&buffer[0] + PROTO_HEADER_SIZE, message.ByteSize())) {
      buffer.clear();
    }
  }
  return buffer;
}


uint16_t GetProtoMessageType(const void* buffer, uint16_t sz) {
  assert(sz >= PROTO_HEADER_SIZE);
  return Poco::ByteOrder::fromNetwork(*(const uint16_t*)buffer);
}
