#pragma once


#include <functional>

#include "game/map/Info.h"
#include "game/player/Info.h"


namespace game::action {


#define GAME_ACTION_PARAMETERS size_t iPlayerNum, game::map::Info& iGameMap, std::vector<game::player::Info>& iAllPlayer
#define GAME_ACTION_ARGUMENTS  iPlayerNum, iGameMap, iAllPlayer
  enum eReturn : uint8_t {
    none,
    move,
    stop,

  };

  using return_t = eReturn;

  class def {
     
    size_t _playerNum;
    game::map::Info& _GameMap; 
    std::vector<game::player::Info>& _AllPlayer;

  public:

    def(GAME_ACTION_PARAMETERS) : _playerNum(iPlayerNum), _GameMap(iGameMap), _AllPlayer(iAllPlayer) {
    }

    game::player::Info& getPlayer() {
      return _AllPlayer[_playerNum];
    }

    virtual ~def(){}
    virtual return_t apply() = 0;
    virtual bool done() const = 0;

    virtual void serialize(network::Buffer& ioBuffer) const = 0;

  };
  

  using action_t = def*;


};