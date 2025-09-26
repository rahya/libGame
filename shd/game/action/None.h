#pragma once

#include "shd/game/action/def.h"


namespace shd::game::action {


  class None : public shd::game::action::def {


    public:
      None(GAME_ACTION_PARAMETERS) : def(GAME_ACTION_ARGUMENTS) {  }

      return_t apply() {

        return shd::game::action::eReturn::none;
      }

      bool done() const override {
        return false;
      }


      void serialize(network::Buffer& ioBuffer) const override {
          ioBuffer.write(game::action::eReturn::none);

      }


      static game::action::def* deserialize(GAME_ACTION_PARAMETERS, network::Buffer& ioBuffer) {
        /*
          _playerPosition.deserialize(ioBuffer);
          _mathVector.deserialize(ioBuffer);
          ioBuffer.read(&_count);
        */
        return nullptr;
      }


  };


};