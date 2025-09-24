#pragma once

#include <vector>
#include <functional>

#include "misc/log.h"
#include "misc/Threadable.h"

#include "network/tcp/Socket.h"



namespace network::tcp {


  class Server : public misc::Threadable {


    Socket _socket;

    static constexpr int NB_MAX_CONNECTION = 100;


    std::function<void(network::tcp::Socket&& iSocket)> _socketExporter;


    void handleNewConnection(fd_set& iFdSet) {
      sockaddr_in newClientSin;
      PLATFORM_TYPE(SOCKTYPE, SOCKADDR, sockaddr);
      socklen_t addrlen = sizeof(sockaddr_in);
      Socket aClientSocket = accept(_socket.get(), (SOCKTYPE*)(&newClientSin), &addrlen);
      if (aClientSocket.isInvalid()) {
        LOG_ERROR(getLogPrefix() << "Failed to accept socket : " << network::tcp::error::get());
        return;
      }
      if (aClientSocket.setBlocking(false) == false) {
        LOG_ERROR(getLogPrefix() << "Failed to set socket in non-blocking mode. Closing socket : " << aClientSocket);
        aClientSocket.close(getLogPrefix());
        return;
      }

      LOG_SOCKET(getLogPrefix() << "Socket : " << aClientSocket.get()<< " (new client)");
      _socketExporter(std::move(aClientSocket));

    }


  public:
    Server(int iPort) {
      _socket.setPort(iPort);

    }
    Server(){}
    ~Server() {
    }

    void setPort(int iPort) {
      _socket.setPort(iPort);
    }

    const char* getLogPrefix() const override {
      return "[Server ] ";
    }

    void setSocketExporter(std::function<void(network::tcp::Socket&& iSocket)> iSocketExporter) {
      _socketExporter = iSocketExporter;
    }

    Socket& getSocket() {
      return _socket;
    }

    bool subInit() override {
      if (_socketExporter == nullptr) {
        LOG_ERROR(getLogPrefix() << "Socket exporter not set");
        return false;
      }

      if (_socket.isValid() == false) {
        _socket.init();
      }
      if (!_socket.bind() || !_socket.listen(NB_MAX_CONNECTION)) {
        LOG_ERROR(getLogPrefix() << "Fail to bind or listen");
        _socket.close(getLogPrefix());
        return false;
      }
      LOG_SOCKET(getLogPrefix() << "Socket : " << _socket.get());
      return true;
    }



    void subRun() override {

      std::unique_ptr<fd_set> afdSet0 = std::unique_ptr<fd_set>(new fd_set);
      std::unique_ptr<fd_set> aFdSet1 = std::unique_ptr<fd_set>(new fd_set);
      FD_ZERO(afdSet0.get());
      FD_SET(_socket.get(), afdSet0.get());
      while (isRunning()) {
        struct timeval aTimeout = misc::platform::varWinLin(timeval { 0, 200000 }, timeval{ 5, 0 });
        *aFdSet1 = *afdSet0;

        int selectReady = select(static_cast<int>(_socket.get() + 1), aFdSet1.get(), nullptr, nullptr, &aTimeout);
        if (selectReady == -1) {
          LOG_ERROR(getLogPrefix() << "Failed select : " << network::tcp::error::get());
          Threadable::setClosing();
        }
        else if (selectReady) {
          handleNewConnection(*aFdSet1);
        }
        //else {} // timeout
      }
      getSocket().close(getLogPrefix());
    }



  };


};