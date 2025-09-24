#pragma once


#include "game/action/def.h"

#include "game/action/None.h"
#include "game/action/Stop.h"
#include "game/action/Move.h"


namespace game::action::tools {


	void serialize(network::Buffer& ioBuffer, const game::action::def* iAction) {
		iAction->serialize(ioBuffer);
	}

	[[ nodiscard ]] game::action::def* deserialize(GAME_ACTION_PARAMETERS, network::Buffer& ioBuffer) {
		game::action::eReturn aAction;
		ioBuffer.read(reinterpret_cast<uint8_t*>(&aAction));
		game::action::def* aActionPtr = nullptr;
		switch (aAction) {
			case game::action::eReturn::none :
				aActionPtr = game::action::None::deserialize(GAME_ACTION_ARGUMENTS, ioBuffer);
				break;

			case game::action::eReturn::move :
				aActionPtr = game::action::Move::deserialize(GAME_ACTION_ARGUMENTS, ioBuffer);
				break;

			case game::action::eReturn::stop :
				aActionPtr = game::action::Stop::deserialize(GAME_ACTION_ARGUMENTS, ioBuffer);
				break;

			default:
				LOG_INFO("Invalid action deserialize : " << aAction);
				aActionPtr = game::action::None::deserialize(GAME_ACTION_ARGUMENTS, ioBuffer);
				break;
		};
		return aActionPtr;
	}


};