#pragma once


namespace misc::platform {


  template<typename T>
  T varWinLin(const T& iWindowsVar, const T& iLinuxVar) {
#ifdef _WIN32
    return iWindowsVar;
#else
    return iLinuxVar;
#endif
  }


};
#ifdef _WIN32
  #define PLATFORM_VALUE(WIN,LIN) WIN
  #define PLATFORM_TYPE(NAME,WIN,LIN) using NAME = WIN
  #define PLATFORM_CALL(WIN,LIN,...) WIN(__VA_ARGS__)
#else

    #define PLATFORM_VALUE(WIN,LIN) LIN
  #define PLATFORM_TYPE(NAME,WIN,LIN) using NAME = LIN
  #define PLATFORM_CALL(WIN,LIN,...) LIN(__VA_ARGS__)
#endif