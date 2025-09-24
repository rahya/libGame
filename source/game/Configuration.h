#pragma once

#include "game/map/Size.h"

namespace game {


#define CREATE_ITEM(TYPE,NAME) \
  private: TYPE _ ## NAME;\
  public:\
  void set ## NAME (const TYPE& i ## NAME){ _ ## NAME = i ## NAME ;}\
  TYPE get ## NAME () const { return _ ## NAME ;}


#define CREATE_ITEM_1(TYPE,NAME,DEFAULT) \
  private: TYPE _ ## NAME = DEFAULT;\
  public:\
  void set ## NAME (const TYPE& i ## NAME){ _ ## NAME = i ## NAME ;}\
  TYPE get ## NAME () const { return _ ## NAME ;} 

#pragma pack(push, 1)
    class Configuration {
        static constexpr const uint32_t _kWidthMax = 200u;
        static constexpr const uint32_t _kHeightMax = 40u;

    public:


        static game::map::Size getMapSizeDefault() {
            return { _kWidthMax, _kHeightMax };
        }

        CREATE_ITEM_1(game::map::Size, MapSize, getMapSizeDefault());

    enum adventureType : uint8_t {
      wander,
      skirmish, // escarmouche
      trap,
    };
    CREATE_ITEM_1(adventureType, AventureType, adventureType::wander);
    CREATE_ITEM_1(uint16_t, ActionSpeedDefault, 200);
    CREATE_ITEM_1(uint16_t, ActionSpeedStep   , 100);
    



  };
#pragma pack(pop)

#undef CREATE_ITEM
};