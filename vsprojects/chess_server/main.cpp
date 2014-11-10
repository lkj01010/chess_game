#ifdef _MSC_VER
#include <vld.h>
#endif
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "chess_server.h"
#include "logger.h"
#include <boost/ptr_container/ptr_vector.hpp>

#ifdef ENABLE_LOG
_INITIALIZE_EASYLOGGINGPP
#endif

class tests : public std::enable_shared_from_this<tests> {
public:
  tests(int v) { v_ = v; 
  }
  ~tests() {
    std::cout << v_ << std::endl;
  }

private:
  int v_;
};


int main(int argc, char* argv[]) {
#ifdef ENABLE_LOG
  el::Loggers::reconfigureAllLoggers(el::Configurations("easylogging.conf"));
#endif
  boost::asio::io_service ios;
  ChessServer server(ios);
  if (server.Start()) {
    CS_LOG_FATAL("server init error");
  }
  boost::thread_group threads;
  threads.create_thread([&]() { 
    while (!ios.stopped()) {
      boost::system::error_code ec;
      ios.run_one(ec);
    }
  });
  threads.join_all();
  return 0;
}
