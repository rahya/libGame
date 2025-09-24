#pragma once

#include <algorithm>

#include "game/Configuration.h"
#include "game/map/Info.h"

namespace game::map {



	class Generator {


	public:

		static void create(game::map::Info& ioMap, const game::Configuration& iConfig) {
			setSize(ioMap, iConfig);

			size_t aConstraintMapSize = ioMap.getSize().width * ioMap.getSize().height;
			ioMap.setConstraintMap(new game::map::Constraint[aConstraintMapSize]);
			for (size_t i = 0; i < aConstraintMapSize; i += 1) {
				assert(ioMap.getConstraintMap()[i].does(game::map::Constraint::eTag::blockPath) == false);
			}
		}


	private:

		static void setSize(game::map::Info& ioMap, const game::Configuration& iConfig) {
			ioMap.setSize(game::map::Size{
				std::min(iConfig.getMapSize().width, game::Configuration::getMapSizeDefault().width),
				std::min(iConfig.getMapSize().height, game::Configuration::getMapSizeDefault().height)
				}
			);
		}

	};


};