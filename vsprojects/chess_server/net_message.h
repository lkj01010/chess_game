#ifndef NET_MESSAGE_H
#define NET_MESSAGE_H

#include <cstdint>
#include <memory>
#include <vector>
#include <google/protobuf/message_lite.h>

/**
 * 类似TCP/IP协议栈，每一层有自己的头部信息
 */



struct PacketHeader {
  uint16_t sz = 0;
};


struct ProtoMessageHeader {
  uint16_t type = -1;
};


/**
 * 将message加上ProtoMessageHeader
 */
std::vector<int8_t> SerializeProtoPacket(const google::protobuf::MessageLite &message,
                                         uint16_t message_type);


uint16_t GetProtoMessageType(const void* buffer, uint16_t sz);


#define PACKET_HEADER_SIZE  sizeof(PacketHeader)
#define PROTO_HEADER_SIZE   sizeof(ProtoMessageHeader)

#endif /* NET_MESSAGE_H */
