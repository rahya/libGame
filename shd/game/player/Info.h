#pragma once

#include "shd/network/tcp/Socket.h"
#include "shd/game/player/Name.h"
#include "shd/game/player/Sheet.h"
#include "shd/game/map/Position.h"

namespace shd::game::player {


  class Info {

    constexpr static const size_t _kSizeName = 16;

    shd::game::player::Name  _name;
    shd::game::player::Sheet _sheet;
    shd::game::map::Position _mapPos;
    

  public:
    Info() {}
    Info(int     iSizeName) : _name(iSizeName){  }
    Info(const char* iName) : _name(iName){ }


    const shd::game::player::Name& getName() const {
      return _name;
    }  
    shd::game::player::Name& getName(){
      return _name;
    }

    shd::game::map::Position& getPosition() { return _mapPos; }
    const shd::game::map::Position& getPosition() const { return _mapPos; }

  };
};