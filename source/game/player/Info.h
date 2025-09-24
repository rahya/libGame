#pragma once

#include "network/tcp/Socket.h"
#include "game/player/Name.h"
#include "game/player/Sheet.h"
#include "game/map/Position.h"

namespace game::player {


  class Info {

    constexpr static const size_t _kSizeName = 16;

    game::player::Name  _name;
    game::player::Sheet _sheet;
    game::map::Position _mapPos;
    

  public:
    Info() {}
    Info(int     iSizeName) : _name(iSizeName){  }
    Info(const char* iName) : _name(iName){ }


    const Name& getName() const {
      return _name;
    }  
    Name& getName(){
      return _name;
    }

    game::map::Position& getPosition() { return _mapPos; }
    const game::map::Position& getPosition() const { return _mapPos; }

  };
};