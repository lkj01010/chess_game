#ifndef GAMEFACTORY_H
#define GAMEFACTORY_H

#include <string>
#include <chess_server/interfaces.h>

typedef IGameLogi* (*create_game_t)(IServerLogi* server_logi);
typedef void(*destroy_game_t)(IGameLogi* instance);

class GameFactory {
public:
  GameFactory(const std::string& dlpath);
  ~GameFactory();

  IGameLogi* CreateGame(IServerLogi* server_logi) {
    return create_game_(server_logi);
  }

  void DestroyGame(IGameLogi *instance) {
    return destroy_game_(instance);
  }

private:
  create_game_t create_game_;
  destroy_game_t destroy_game_;
};

#endif /* GAMEFACTORY_H */
