#pragma once

#include "game/action/def.h"







namespace game::action {


  class Stop : public game::action::def {


  public:

    Stop(GAME_ACTION_PARAMETERS) : def(GAME_ACTION_ARGUMENTS) {}

    return_t apply() override {

      return game::action::eReturn::stop;

    }

    bool done() const override {
      return true;
    }



    void serialize(network::Buffer& ioBuffer) const override {
        ioBuffer.write(game::action::eReturn::stop);

    }

    static game::action::def* deserialize(GAME_ACTION_PARAMETERS, network::Buffer& ioBuffer) {

      return nullptr;
    }


  };



};