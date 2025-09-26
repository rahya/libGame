#pragma once

#include "shd/game/map/Constraint.h"

#include "shd/game/map/Size.h"

namespace shd::game::map {


	class Info {


	public:
	  Info() = default;

	  Info(const Info& iMap) = delete;
		void operator=(const Info&) = delete;

		~Info() {
			destroy();
		}

		void setSize(const shd::game::map::Size iSize) { _size = iSize; }
		shd::game::map::Size getSize() const { return _size; }
		shd::game::map::Size& getSize() { return _size; }


		void setConstraintMap(shd::game::map::Constraint* iConstraintMap) {
			_constraintMap = iConstraintMap;
		}
		const shd::game::map::Constraint* getConstraintMap() const {
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
	  shd::game::map::Size _size = { 0ULL, 0ULL };
	  shd::game::map::Constraint* _constraintMap = nullptr;

	};


};