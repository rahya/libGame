#pragma once

#include <stdio.h>
#include <string.h>


namespace shd::game::player {


  class Name {


    static constexpr const size_t _memorySize = 16;
  public :
    static constexpr const size_t getMemorySize() {
      return _memorySize;
    }

  private:
    char name[_memorySize] = {0};

  public:


    Name() {}
    Name(int iIntName) {
      snprintf(name, getMemorySize(), "%d", iIntName);

    }
    Name(const char* iName) {
      memcpy(name, iName, getMemorySize());
      name[strlen(iName)] = '\0';
    }

    void set(int iSizeName) {
      snprintf(name, getMemorySize(), "%d", iSizeName);
    }

    void set(const char* iName) {
      snprintf(name, getMemorySize(), "%s", iName);
    }

    operator const char* () {
      return name;
    }

    const char* data() const {
      return name;
    }

  };
};