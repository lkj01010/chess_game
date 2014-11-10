#include <Poco/ByteOrder.h>
#include "Session.h"

using boost::asio::buffer;
using boost::system::error_code;

Session::Session(socket_t &socket)
  : socket_(std::move(socket)), timer_(socket.get_io_service()) {
  closed_ = false;
}


void Session::Init() {
  SetTimer();
  ReadHead();
}


Session::~Session() {
}


void Session::ReadHead() {
  if (closed_) { return; }
  SessionWeakPtr wself(shared_from_this());
  async_read(socket_, buffer(&message_size_, sizeof(message_size_)),
    [wself](error_code ec, std::size_t size) {
    SessionPtr self = wself.lock();
    if (!self) { return; }
    if (self->closed_) { return; }
    if (!ec) {
      self->SetTimer();
      self->ReadBody();
    } else {
      self->OnError();
    }
  });
}


void Session::ReadBody() {
  message_size_ = Poco::ByteOrder::fromNetwork(message_size_);
  const uint16_t body_length = message_size_ - 2;
  if (body_length) {
    SessionWeakPtr wself(shared_from_this());
    message_.reset(new char[body_length]);
    async_read(socket_, buffer(message_.get(), body_length),
      [wself](error_code ec, std::size_t size) {
      SessionPtr self = wself.lock();
      if (!self) { return; }
      if (self->closed_) { return; }
      if (!ec) {
        if (self->on_message_cb_) {
          self->on_message_cb_(self->message_.get(), self->message_size_ - 2); 
        }
        self->SetTimer();
        self->ReadHead();
      } else {
        self->OnError();
      }
    });
  } else {
    message_.reset();
    ReadHead();
  }
}


void Session::SendMessage(const void* data, uint16_t sz) {
  if (!sz) { return; }
  if (!socket_.is_open() || closed_) { return; }
  const uint16_t wire_sz = 2 + sz;
  char* wire_data = (char*)malloc(wire_sz);
  *(uint16_t*)wire_data = Poco::ByteOrder::toNetwork(wire_sz);
  memcpy(&wire_data[2], data, sz);
  SessionWeakPtr wself(shared_from_this());
  WireBufferPtr wire_buffer = std::make_shared<WireBuffer>(wire_data, sz + 2);
  socket_.get_io_service().post([wself, wire_buffer]() {
    SessionPtr self = wself.lock();
    if (!self) { return; }
    if (self->closed_) { return; }
    bool is_empty = self->send_queue_.empty();
    self->send_queue_.push(wire_buffer);
    if (is_empty) { self->DoWrite(); }
  });
}


void Session::DoWrite() {
  SessionWeakPtr wself(shared_from_this());
  async_write(socket_, buffer(send_queue_.front()->data, send_queue_.front()->sz),
    [wself](boost::system::error_code ec, std::size_t /*length*/) {
    SessionPtr self = wself.lock();
    if (!self) { return; }
    if (!ec) {
      self->send_queue_.pop();
      if (!self->send_queue_.empty()) {
        self->DoWrite();
      }
    } else {
      self->OnError();
    }
  });
}


void Session::OnError() {
  if (!closed_) {
    Disconnect();
    if (on_close_cb_) { on_close_cb_(); }
  }
}


void Session::Disconnect() {
  if (closed_) { return; }
  closed_ = true;
  error_code ec;
  if (socket_.is_open()) { socket_.close(ec); }
  timer_.cancel(ec);
}


void Session::SetTimer() {
  SessionWeakPtr wself(shared_from_this());
  error_code ec;
  timer_.cancel(ec);
  if (closed_) { return; }
  timer_.expires_from_now(boost::posix_time::seconds(5));
  timer_.async_wait([wself](const error_code &ec) {
    if (ec) { return; }
    SessionPtr self = wself.lock();
    if (!self) { return; }
    self->OnTimeout();
  });
}


void Session::OnTimeout() {
  if (!closed_ && on_timeout_cb_) { on_timeout_cb_(); }
  if (!closed_) { SetTimer(); }
}
