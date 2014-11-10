#ifndef CHESS_SERVER_GATE_H
#define CHESS_SERVER_GATE_H

#include <boost/asio.hpp>
#include <cstdint>
#include <functional>

using boost::asio::io_service;
using boost::asio::ip::tcp;

class Gate {
public:
  typedef std::function<void(tcp::socket &socket)> on_conn_cb_t;

  Gate(boost::asio::io_service &ios);
  ~Gate();

  int Start(uint16_t port);
  void Stop();

  void set_on_conn(const on_conn_cb_t& cb) {
    on_conn_ = cb;
  }

private:
  void AsyncAccept();

private:
  boost::asio::io_service &ios_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
  on_conn_cb_t on_conn_;
};

#endif /* CHESS_SERVER_GATE_H */
