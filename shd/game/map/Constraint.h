#pragma once

#include <cstdint>
#include <cassert>


namespace shd::game::map {




  class Constraint {


    private:
      uint8_t _constr = 0;

 

    public:

      enum eTag : uint8_t {
        blockSight,
        blockProjectible,
        blockPath,
      };

      void setBlock(const eTag iTag) {
        int aBitPos = static_cast<int>(iTag);
        assert(aBitPos < static_cast<int>(sizeof(_constr) * 8));
        _constr = _constr | (1 << aBitPos);
      }

      bool does(const eTag iTag) const {
        return get(iTag);
      }

  protected:
    bool get(const eTag iTag) const {
      int aBitPos = static_cast<int>(iTag);
      assert(aBitPos < static_cast<int>(sizeof(_constr) * 8));
      return _constr & (1 << aBitPos);
    }



  };



};