#ifndef CHESS_SERVER_H
#define CHESS_SERVER_H

#include <vector>
#include <cstdint>
#include <memory>
#include <set>
#include "gate.h"
#include "game_user.h"
#include "game_table.h"
#include "game_factory.h"
#include "session.h"
#include "chess_server_proto.h"

using boost::asio::io_service;

class ChessServer {
  enum {
    SEAT_COUNT = 3,
    DEFAULT_TABLE_COUNT = 100
  };

public:
  ChessServer(io_service &ios);
  ~ChessServer();
  int Start();

private:
  void Stop();
  void OnConnection(tcp::socket &socket);

  // unauth session handler
  void OnTimeout(SessionWeakPtr wsession);
  void OnClose(SessionWeakPtr wsession);
  void OnMessage(SessionWeakPtr wsession, const void* data, uint16_t sz);
  void HandleLogin(SessionPtr session, const CmdLogin* login);

  // auth session handler
  void OnTimeout(GameUserPtr user);
  void OnClose(GameUserPtr user);
  void OnMessage(GameUserPtr user, const void* data, uint16_t sz);
  void HandleSitdown(GameUserPtr user, const CmdSitDown *sitdown);
  void HandleRandSitdown(GameUserPtr user);

  void RemoveUser(GameUserPtr user) {
    auto iter = users_.find(user);
    if (iter != users_.end()) { users_.erase(iter); }
  }

  void RemoveUser() { users_.clear(); }

  void ForwardTableMessage(GameUserPtr user, uint8_t type, const void* data, uint16_t sz);

  void SendMessageTo(GameUserPtr to, uint8_t type, const void* data = nullptr, uint16_t sz = 0);
  void SendMessageExcept(GameUserPtr except, uint8_t type, const void* data = nullptr, uint16_t sz = 0);
  void SendMessageToAll(uint8_t type, const void* data = nullptr, uint16_t sz = 0) {
    SendMessageExcept(nullptr, type, data, sz);
  }

  GameFactory* GetGameFactory() const {
    return game_factory_.get();
  }

private:
  io_service &ios_;
  Gate gate_;
  std::unique_ptr<GameFactory> game_factory_;
  GameTable tables_[DEFAULT_TABLE_COUNT];
  std::set<SessionPtr> unauth_sessions_;
  std::set<GameUserPtr> users_;
  friend class GameTable;
};

#endif /* CHESS_SERVER_H */
