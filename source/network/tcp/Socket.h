#pragma once



#ifdef _WIN32
#include <ws2tcpip.h>
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <typeinfo>

#include "network/tcp/error.h"
#include "network/message/error.h"
#include "network/buffer/Output.h"
#include "network/buffer/Input.h"

#include "misc/log.h"
#include "misc/platform.h"


//define LOG_SOCKET(VAR) LOG_INFO("[socket]" << VAR)
#define LOG_SOCKET(VAR)

namespace network::tcp {




  class Socket {

#ifdef _WIN32
#define socket_t SOCKET
#else
#define socket_t int
#endif

#define RETURN_IF_SOCKET_INVALID  if (!isValid()) { LOG_ERROR("Socket is NOT initialized : " << __FILE__ << " " << __LINE__); return false; }
#define RETURN_IF_SOCKET_INVALID_(VALUE)  if (!isValid()) { LOG_ERROR("Socket is NOT initialized : " << __FILE__ << " " << __LINE__); return VALUE; }

    socket_t _socket = std::numeric_limits<int>::max();
    sockaddr_in _sin = { 0 };


    Socket(socket_t iSocket) : _socket(iSocket) {}

    socket_t get() { return _socket; }

    static constexpr int _FAMILY = AF_INET;
    int getFamily() {
      return _FAMILY;
    }


    bool write(const char* iByteArray, const size_t iSize) {
      RETURN_IF_SOCKET_INVALID;
      LOG_MESSAGE("[Socket] Send : " << static_cast<int>(((network::message::Header*)iByteArray)->code) << " -- Payload size : " << ((network::message::Header*)iByteArray)->payloadSize);
      int iResult = send(_socket, iByteArray, static_cast<int>(iSize), PLATFORM_VALUE(0, MSG_NOSIGNAL));
      if (isError(iResult)) {
        LOG_ERROR("Send failed with error : " << network::tcp::error::get());
        close();
        return false;
      }
      return true;
    }

  public:

    static bool isError(int iValue) {
      return iValue == -1;
    }
    void setInvalid() {
      _socket = std::numeric_limits<int>::max();
    }
    bool isInvalid() const {
      return _socket == std::numeric_limits<int>::max();
    }

    int getNum() const {
      return static_cast<int>(_socket);
    }

    Socket() {
    }

    Socket(const Socket& iSocket) = delete;
    Socket(Socket& iSocket) = delete;
    Socket(Socket&& iSocket) noexcept {
      _socket = iSocket._socket;
      iSocket.setInvalid();
    }

    void operator=(Socket&& iSocket) noexcept {
      if (isValid()) {
        LOG_ERROR("Call socket.operator= will remove valid socket : " << _socket);
      }
      _socket = iSocket._socket;
      _sin = iSocket._sin;
    }

    void init() {
      _sin.sin_family = getFamily();
      _socket = socket(getFamily(), SOCK_STREAM, IPPROTO_TCP);
      if (!isValid()) {
        LOG_ERROR("Failed to create socket " << errno << " : " << network::tcp::error::get());
      }
    }

    bool isValid() const {
      return !isInvalid();
    }

    void setAddrAny() {
      _sin.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    void setAddrLoopBack() {
      _sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }
    void setAddr(unsigned long iAddr) {
      _sin.sin_addr.s_addr = htonl(iAddr);
    }
    void setAddr(const std::string_view iAddrString) {
      uint32_t aBinaryIp;
      int aResult = inet_pton(getFamily(), iAddrString.data(), &aBinaryIp);
      if (isError(aResult)) {
        LOG_ERROR("Failed call inet_pton : " << network::tcp::error::get());
        close();
        return;
      }
      else if (aResult == 0) {
        LOG_ERROR("Input is not a valid IPv4 dotted-decimal string : " << iAddrString);
        close();
        return;
      }
      _sin.sin_addr.s_addr = aBinaryIp;
    }

    void setPort(uint16_t iPort) {
      _sin.sin_port = htons(iPort);
    }

    void close(const std::string& iCaller = "") {
      if (isInvalid() == false) {
        if (iCaller.empty() == false) {
          LOG_SOCKET("Client : " << iCaller << " -- Close socket : " << _socket);
        }
        PLATFORM_CALL(closesocket, ::close, _socket);
        setInvalid();
      }
    }

#ifdef __linux__
    bool setOption(const char* iInfo, socket_t& iSocket, int iOption, int iValue) {
        LOG_SOCKET("Socket option : " << iInfo);
      if (setsockopt(iSocket, SOL_SOCKET, iOption, &iValue, sizeof(int)) == -1) {
        LOG_ERROR("Failed set socket option " << iOption << " to " << iValue << " : " << network::tcp::error::get());
        return false;
      }
      return true;
    }

#endif

    bool bind() {
      RETURN_IF_SOCKET_INVALID;
      PLATFORM_TYPE(SOCKTYPE, SOCKADDR, sockaddr);
#ifdef __linux__
      setOption("SO_REUSEADDR", _socket, SO_REUSEADDR, 1);
      setOption("SO_REUSEPORT", _socket, SO_REUSEPORT, 1);
#endif
      if (isError(::bind(_socket, (SOCKTYPE*)&_sin, sizeof(_sin)))) {
        LOG_ERROR("Failed to bind socket : " << network::tcp::error::get());
        close();
        return false;
      }
      return true;
    }

    bool listen(int iNbMaxConnection) {
      RETURN_IF_SOCKET_INVALID;
      if (isError(::listen(_socket, iNbMaxConnection))) {
        LOG_ERROR("Failed to listen socket : " << network::tcp::error::get());
        close();
        return false;
      }
      return true;
    }


    bool connect() {
      RETURN_IF_SOCKET_INVALID;
      PLATFORM_TYPE(SOCKTYPE, SOCKADDR, sockaddr);
      int connectStatus = ::connect(_socket, (SOCKTYPE*)&_sin, sizeof(_sin)); //function to connect to the server
      if (isError(connectStatus)) { //it returns 0 if no error occurs
        LOG_ERROR("Failed to connect : " << network::tcp::error::get());
        close();
        return false;
      }
      return true;
    }


    bool writeError(const network::message::Header::id iType) {
      network::message::error::text aText;
      auto [aMessage, aMessageSize] = network::message::error::create(iType, aText);
      return write(aMessage, aMessageSize);
    }

    bool write(network::buffer::Output& iBuffer) {
      return write(iBuffer, sizeof(network::message::Header) + iBuffer.getHeader()->getPayloadSize());
    }

    bool write(const network::buffer::Output& iBuffer, const size_t iSize) {
      RETURN_IF_SOCKET_INVALID;
      LOG_SOCKET("[Socket] Send : " << *iBuffer.getHeader());
      int iResult = send(_socket, iBuffer, static_cast<int>(iSize), PLATFORM_VALUE(0, MSG_NOSIGNAL));
      if (isError(iResult)) {
        LOG_ERROR("Send failed with error : " << network::tcp::error::get());
        close();
        return false;
      }
      return true;
    }

    std::string getLogPrefix() const {
      return "[socket ] ";
    }

    std::pair<bool, int> read(const char* iCallerInfo, char* oResult, const size_t iReadSize)
    {
      std::pair<bool, int> aDefaultResult = { false, 0 };
      RETURN_IF_SOCKET_INVALID_(aDefaultResult);
      int aNbCharRead = recv(_socket, oResult, static_cast<int>(iReadSize), 0);
      LOG_MESSAGE("[Socket] Read : " << static_cast<int>(((network::message::Header*)oResult)->code) << " -- Payload size : " << ((network::message::Header*)oResult)->payloadSize);
      if (isError(aNbCharRead)) {
        network::tcp::error::error_t aError = network::tcp::error::get();
        if (aError.first == misc::platform::varWinLin(10035, 11)) {
          writeError(network::message::Header::id::HeaderOnly);
          LOG_ERROR(iCallerInfo << "Header only message");
        } else if (aError.first == misc::platform::varWinLin(10, 104)) {
          LOG_ERROR(iCallerInfo << " -- " << network::tcp::error::get());
        }
        else {
          LOG_ERROR(iCallerInfo << "Failed to recv : " << network::tcp::error::get());
          writeError(network::message::Header::id::RecvFailed);
        }
        close(getLogPrefix());
        return aDefaultResult;
      }
      else if (aNbCharRead == 0) {
        LOG_INFO(iCallerInfo << " Socket closed by remote : " << _socket);
        close();
        return { false, 1};
      }
      else if (aNbCharRead != static_cast<int>(iReadSize)) {
        LOG_INFO(iCallerInfo << "Socket read did not fill the structure : " << aNbCharRead << " != " << iReadSize << " " << iCallerInfo << " - Socket closed : " << _socket);
        writeError(network::message::Header::id::InvalidRead);
        close();
        return  { true, aNbCharRead };
        //auto [aResult, aNbCharRead1] = read(iCallerInfo, &(oResult[aNbCharRead]), iReadSize - aNbCharRead);
        //return { aNbCharRead1 + aNbCharRead == iReadSize, aNbCharRead1 + aNbCharRead };
      }
      return { true, aNbCharRead };
    }

    bool read(const char* iCallerInfo, network::buffer::Input &ioBuffer)
    {
      ioBuffer.reset();
      {
        auto [ aSuccess, aSize ] = read(iCallerInfo, ioBuffer.getData(), sizeof(::network::message::Header));
        if (!aSuccess || aSize != sizeof(::network::message::Header)) {
          return false;
        }
      }
      if(ioBuffer.getHeader()->getPayloadSize() != 0){
        auto [aSuccess, aSize] = read(iCallerInfo, ioBuffer.getPayload(), ioBuffer.getHeader()->getPayloadSize());
        if (!aSuccess || static_cast<size_t>(aSize) != ioBuffer.getHeader()->getPayloadSize()) {
          return false;
        }
      }
      return true;
    }

    std::pair<bool, int> readString(const size_t iOutputStringSize, network::buffer::Input& aBuffer) {
      auto aResult = read("ReadString", aBuffer, iOutputStringSize);
      aBuffer[aResult.second] = '\0';
      return aResult;
    }

    bool setBlocking(bool blocking)
    {
      RETURN_IF_SOCKET_INVALID
#ifdef _WIN32
        unsigned long mode = blocking ? 0 : 1;
      return (ioctlsocket(_socket, FIONBIO, &mode) == 0);
#else
        int flags = fcntl(_socket, F_GETFL, 0);
      if (flags == -1) {
        return false;
      }
      flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
      return (fcntl(_socket, F_SETFL, flags) == 0);
#endif
    }

    friend std::ostream& operator<<(std::ostream& oStream, const Socket& iSocket) {
      oStream << iSocket._socket;
      return oStream;
    }


  private:

    friend class Server;
    friend class Client;
    friend class Poll;




  };




};