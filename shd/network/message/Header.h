#pragma once

#include <cstdint>
#include <iostream>


namespace shd::network::message {

#pragma pack(push, 1)
  struct Header {


    enum class id : uint8_t {
      endianess,
      description,
      rename,
      chat,
      lobbyCreate,
      lobbyJoin,
      lobbyList,
      lobbyQuit,
      phaseChange,
      acknowledege,

      //gameConfigMapSize,
      //gameConfigType,

      gameStart,
      gameStop,
      gamePlayerMoveVector,
      gamePlayerMoveStop,

      __last_message,

      __begin_error = 99,
      ServerFull,
      LobbyFull,
      InvalidRead,
      InvalidString,
      RecvFailed,
      HeaderOnly,
      LobbyPrivateNotFound,
      
      __last_error

    };

    uint8_t code = 0;
    uint16_t payloadSize = 0;
    Header(){}
    Header(id iCode) :
      code(getId(iCode)),
      payloadSize(static_cast<uint16_t>(0))
    {
    }
    Header(id iCode, int iPayloadSize) : 
      code(getId(iCode)),
      payloadSize(static_cast<uint16_t>(iPayloadSize))
    {}
    Header(id iCode, size_t iPayloadSize) :
      code(getId(iCode)),
      payloadSize(static_cast<uint16_t>(iPayloadSize))
    {}

    id getCode() const {
      return static_cast<id>(code);
    }
    void setCode(id iId) {
      code = static_cast<uint8_t>(iId);
    }

    void setPayloadSize(const size_t iSize) {
      payloadSize = static_cast<uint16_t>(iSize);
    }
    size_t getPayloadSize() const {
      return payloadSize;
    }

    void set(const id iId, const size_t iSize) {
      setCode(iId);
      setPayloadSize(iSize);
    }

    size_t getMemorySize() const {
      return sizeof(Header);
    }

    friend std::ostream& operator<<(std::ostream& iOut, const Header& iHeader) {
      iOut << "{" << static_cast<int>(iHeader.code) << "," << iHeader.payloadSize << "}";
      return iOut;
    }

  private :

    static uint8_t getId(network::message::Header::id iId) {
      return static_cast<uint8_t>(iId);
    }

  };
#pragma pack(pop)


};