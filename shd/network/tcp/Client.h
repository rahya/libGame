#pragma once


#ifdef _WIN32
  #include <ws2tcpip.h>
#else
  #include <netinet/in.h>
#endif

#include "shd/network/tcp/Socket.h"



namespace shd::network::tcp {

  class Client : public Socket {

    //Socket _socket;

  public:
    Client() {

    }

    std::string getLogPrefix() const {
      return "[Client ] ";
    }

    void init() {
      Socket::init();
      LOG_SOCKET("[Client ] Socket : " << get());
    }

    ~Client() {
      if (isValid()) {
        close(getLogPrefix());
      }
      
    }



  private:




  };


};