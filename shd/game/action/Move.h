#pragma once

#include <cstdint>

#include "shd/game/action/def.h"
#include "shd/math/Vector.h"




namespace shd::game::action {

  class Move : public shd::game::action::def {


    game::map::Position _playerPosition;
    math::Vector        _mathVector;
    size_t              _count = 0;

  public:
    Move(GAME_ACTION_PARAMETERS, const math::Vector& iMathVector) 
      : def(GAME_ACTION_ARGUMENTS), _playerPosition(iAllPlayer[iPlayerNum].getPosition()), _mathVector(iMathVector) {

    }

    return_t apply() override {
      _count += 1;
      getPlayer().getPosition() = _mathVector.compute(_playerPosition, _count);
      //LOG_INFO("Player moove " << _mathVector.compute(_playerPosition, _count) << " " << _mathVector);
      return game::action::eReturn::move;
    }

    bool done() const override {
      return _count > static_cast<size_t>(_mathVector.size());
    }



    void serialize(network::Buffer& ioBuffer) const override{
        ioBuffer.write(game::action::eReturn::move);
        _playerPosition.serialize(ioBuffer);
        _mathVector.serialize(ioBuffer);
        ioBuffer.write(_count);
    }

    static game::action::def* deserialize(GAME_ACTION_PARAMETERS, network::Buffer& ioBuffer)  {
      /*
        _playerPosition.deserialize(ioBuffer);
        _mathVector.deserialize(ioBuffer);
        ioBuffer.read(&_count);
      */
        return nullptr;
    }


  };

};