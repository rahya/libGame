#pragma once

#include <math.h>
#include <cstdint>

namespace shd::math {

	class Vector {


	public:
		using type_t = int32_t;
		Vector() = default;
		Vector(const int32_t iDx, const int32_t iDy) : _dx(iDx), _dy(iDy) {
			_sizeq = std::sqrt(_dx * _dx + _dy * _dy);
		}

		~Vector() {}

		void set(int32_t iDx, int32_t iDy) {
			_dx = iDx;
			_dy = iDy;
			_sizeq = std::sqrt(_dx * _dx + _dy * _dy);
		}

		void operator=(const Vector& iVector) {
			_dx = iVector.getDx();
			_dy = iVector.getDy();
			_sizeq = iVector.size();
		}

		bool operator==(const Vector& iVector) const {
			return _dx == iVector.getDx() && _dy == iVector.getDy();
		}

		double size() const {
			return _sizeq;
		}

		shd::game::map::Position compute(const shd::game::map::Position iPosition, size_t iStep) const {
			return {
			  static_cast<uint32_t>(iPosition.x + (iStep * getDx()) / size()),
			  static_cast<uint32_t>(iPosition.y + (iStep * getDy()) / size())
			};
		}

		friend std::ostream& operator<<(std::ostream& ioOut, const Vector& iVector) {
			ioOut << "math::Vector{ " << iVector.getDx() << ", " << iVector.getDy() << ", " << iVector.size() << " }";
			return ioOut;
		}

		int32_t getDx() const { return _dx; }
		int32_t getDy() const { return _dy; }

		void serialize(shd::network::buffer::Output& ioBuffer) const {
			ioBuffer.write(_dx);
			ioBuffer.write(_dy);
		}

		void deserialize(shd::network::buffer::Input& ioBuffer) {
			ioBuffer.read(_dx);
			ioBuffer.read(_dy);
		}

	private:

		type_t _dx = 0;
		type_t _dy = 0;

		double _sizeq = 0;

	};



};

