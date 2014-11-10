#include "Gate.h"

using boost::system::error_code;

Gate::Gate(boost::asio::io_service &ios) : ios_(ios), acceptor_(ios), socket_(ios) {
}


Gate::~Gate() {
  Stop();
}


int Gate::Start(uint16_t port) {
  tcp::acceptor acceptor(ios_);
  tcp::endpoint endpoint(tcp::v4(), port);
  error_code ec;
  acceptor.open(endpoint.protocol(), ec);
  if (ec) { return -1; }
  acceptor.bind(endpoint, ec);
  if (ec) { return -1; }
  acceptor.listen(tcp::acceptor::max_connections, ec);
  if (ec) { return -1; }
  acceptor_ = std::move(acceptor);
  AsyncAccept();
  return 0;
}


void Gate::Stop() {
  error_code ec;
  acceptor_.close(ec);
  socket_.close(ec);
}


void Gate::AsyncAccept() {
  acceptor_.async_accept(socket_, [&](const error_code &ec) {
    if (!ec) {
      if (on_conn_) {
        tcp::socket sock(std::move(socket_));
        on_conn_(sock);
      }
    }
    AsyncAccept();
  });
}
