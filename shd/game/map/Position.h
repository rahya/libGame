#pragma once

#include <cstdint>


namespace shd::game::map {



  struct Position {
      using type_t = uint32_t;
      static constexpr const type_t _kInvalidValue = std::numeric_limits<type_t>::max();

      type_t x = _kInvalidValue;
      type_t y = _kInvalidValue;

      operator bool() {
          return x != _kInvalidValue && y != _kInvalidValue;
      }

      void setInvalid() {
          x = _kInvalidValue;
          y = _kInvalidValue;
      }

      friend std::ostream& operator<<(std::ostream& ioOut, const Position& iPos) {
        ioOut << "game::map::Position{ " << iPos.x << ", " << iPos.y << " }";
        return ioOut;
      }

      void serialize(shd::network::buffer::Output& ioBuffer) const {
          ioBuffer.write(x);
          ioBuffer.write(y);
      }

      void deserialize(shd::network::buffer::Input& ioBuffer) {
          ioBuffer.read(x);
          ioBuffer.read(y);
      }



  };

};