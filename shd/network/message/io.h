#pragma once


#include <array>

#include "shd/network/message/Header.h"
//#include "data/ClientInfo.h"

//#define __LOG_MSG__ 1

#ifdef __LOG_MSG__
#define LOG_MESSAGE(VAR) LOG_INFO(VAR)
#else
#define LOG_MESSAGE(VAR) 
#endif

namespace shd::network::message::io {

  /*
  void send(std::vector<ClientInfo>& iVecClient, network::Buffer& ioBuffer) {
    for (auto& aClient : iVecClient) {
      aClient.getSocket().write(ioBuffer);
    }
  }
  */



  bool checkString(const char* iInputString, const size_t iSizeMax) {
    for (size_t i = 0; i < iSizeMax; i += 1) {
      char aChar = iInputString[i];
      if (aChar == '\0') {
        return i != 0;
      }
      if (aChar < ' ' || aChar > '~') {
        return false;
      }
    }
    return true;
  }

};