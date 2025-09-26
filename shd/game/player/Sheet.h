#pragma once

#include <cstdint>


namespace shd::game::player {



#define CREATE_ITEM(TYPE,NAME,DEFAULT) \
  private: TYPE _ ## NAME = DEFAULT;\
  public:\
  void set ## NAME (const TYPE& i ## NAME){ _ ## NAME = i ## NAME ;}\
  TYPE get ## NAME () const { return _ ## NAME ;}

  class Sheet {

    CREATE_ITEM(int32_t, SpeedMoving, 100);

  };


#undef CREATE_ITEM

};