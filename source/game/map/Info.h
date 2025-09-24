#pragma once

#include "game/map/Constraint.h"

#include "game/map/Size.h"

namespace game::map {


	class Info {


	public:
	  Info() = default;

	  Info(const Info& iMap) = delete;
		void operator=(const Info&) = delete;

		~Info() {
			destroy();
		}

		void setSize(const game::map::Size iSize) { _size = iSize; }
		game::map::Size getSize() const { return _size; }
		game::map::Size& getSize() { return _size; }


		void setConstraintMap(game::map::Constraint* iConstraintMap) {
			_constraintMap = iConstraintMap;
		}
		const game::map::Constraint* getConstraintMap() const {
			return _constraintMap;
		}

		bool isValid() const {
			return getConstraintMap() != nullptr;
		}

		void destroy() {
			if (_constraintMap) {
				delete[] _constraintMap;
				_constraintMap = nullptr;
			}
		}



	protected:
		game::map::Size _size = { 0ULL, 0ULL };
		game::map::Constraint* _constraintMap = nullptr;

	};


};