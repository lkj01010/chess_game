#ifndef GAMELOGI_H
#define GAMELOGI_H

#include <vector>
#include <string>
#include <queue>
#include <cstdint>
#include <thread>
#include <mutex>
#include <memory>
#include <map>
#include <boost/asio.hpp>
#include "session.h"
#include "GameTable.h"
#include "GameUser.h"
#include "chess_server_proto.h"

using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::system::error_code;
typedef tcp::socket socket_t;

class GameLogi {
public:
  virtual ~GameLogi() {}
  virtual void OnUserEnter(GameUser* user) = 0;
  virtual void OnUserStateChange(GameUser* user) = 0;
  virtual void OnUserLeave(GameUser* user) = 0;
  virtual void HandleMessage(const void* data, uint16_t sz) = 0;
};


class ConnectionMonitor {
public:
  virtual ~ConnectionMonitor() {}
  /** 与服务器连接成功 */
  virtual void OnConnected() = 0;

  /** 登录成功 */
  virtual void OnLogin() = 0;

  /** 与服务器连接断开（比如服务器关机、超时未收到服务器心跳、连接失败）*/
  virtual void OnDisconnected(bool connected) = 0;
};


class GameClient {
public:
  GameClient();
  ~GameClient();
  void Start(const char *host, const char *port);
  void Stop();
  void SendLogin(const std::string &uid, const std::string &passwd);
  void SendSeatDown(int table, int seat);
  void SendRandSeatDown();
  void SendReady();
  void SendStandup();
  void SendCommand(const void* data, uint16_t sz);

  void SetMonitor(ConnectionMonitor* monitor) {
    std::lock_guard<std::mutex> lock(mutex_);
    connection_monitor_ = monitor;
  }

  void SetGameLogi(GameLogi* game_logi) {
    std::lock_guard<std::mutex> lock(mutex_);
    game_logi_ = game_logi;
  }

  GameUserPtr GetUser(uint32_t uid) {
    auto iter = users_.find(uid);
    if (iter != users_.end()) { return iter->second; }
    return nullptr;
  }

  GameUserPtr GetMyself() {
    return GetUser(my_user_id_);
  }

private:
  void OnConnectCallback(error_code ec, tcp::resolver::iterator);
  void SendMessage(uint8_t type, const void* data = nullptr, uint16_t sz = 0);
  void HandleMessage(const void* data, uint16_t sz);
  void HandleLogin(const CmdLoginResponse* response);
  void HandleAddUser(const CmdAddUser* adduser);
  void HandleUserSitdown(const CmdTableUserEnter* tue);
  void HandleUpdateUserState(const CmdUpdateUserState* uus);
  void HandleGame(const void* data, uint16_t sz);

private:
  io_service ios_;
  socket_t socket_;
  std::thread thread_;
  SessionPtr session_;

  std::mutex mutex_;
  ConnectionMonitor *connection_monitor_ = nullptr;
  GameLogi* game_logi_ = nullptr;

  uint32_t my_user_id_;
  GameTable tables_[100];
  std::map<uint32_t, GameUserPtr> users_;

  friend class HelloWorld;
};

#endif /* GAMELOGI_H */
