#include <Poco/SharedLibrary.h>
#include "game_factory.h"


GameFactory::GameFactory(const std::string& dlpath) {
  Poco::SharedLibrary sl;
  sl.load(dlpath);
  create_game_ = (create_game_t)sl.getSymbol("CreateGame");
  destroy_game_ = (destroy_game_t)sl.getSymbol("DestroyGame");
}


GameFactory::~GameFactory() {
}
