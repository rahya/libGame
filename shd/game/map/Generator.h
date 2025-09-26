#pragma once

#include <algorithm>

#include "shd/game/Configuration.h"
#include "shd/game/map/Info.h"

namespace shd::game::map {



	class Generator {


	public:

		static void create(shd::game::map::Info& ioMap, const shd::game::Configuration& iConfig) {
			setSize(ioMap, iConfig);

			size_t aConstraintMapSize = ioMap.getSize().width * ioMap.getSize().height;
			ioMap.setConstraintMap(new shd::game::map::Constraint[aConstraintMapSize]);
			for (size_t i = 0; i < aConstraintMapSize; i += 1) {
				assert(ioMap.getConstraintMap()[i].does(game::map::Constraint::eTag::blockPath) == false);
			}
		}


	private:

		static void setSize(shd::game::map::Info& ioMap, const shd::game::Configuration& iConfig) {
			ioMap.setSize(shd::game::map::Size{
				std::min(iConfig.getMapSize().width, shd::game::Configuration::getMapSizeDefault().width),
				std::min(iConfig.getMapSize().height, shd::game::Configuration::getMapSizeDefault().height)
				}
			);
		}

	};


};