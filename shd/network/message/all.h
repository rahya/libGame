#pragma once


#include "shd/lobby/Key.h"

#include "shd/game/Configuration.h"
#include "shd/game/map/Size.h"
#include "shd/game/map/Position.h"

#include "shd/game/player/Info.h"

#include "shd/math/Vector.h"

#include "shd/network/tcp/Socket.h"
#include "shd/network/buffer/Output.h"
#include "shd/network/buffer/Input.h"


namespace shd::network::message {


  struct Description {

    const char* inputDetails;
    const char* processDetails;

    Description(const char* iInput, const char* iProc)
      : inputDetails(iInput), processDetails(iProc)
    {
    }


    static shd::network::buffer::Output& setQuery(shd::network::buffer::Output& ioBuffer) {
        ioBuffer.reset(shd::network::message::Header::id::description);
        return ioBuffer;
    }

    static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, const char* iDescription) {
        ioBuffer.reset(shd::network::message::Header::id::description);
        ioBuffer.write(iDescription);
        return ioBuffer;
    }

    static std::tuple<bool, const char*> getResponse(shd::network::buffer::Input& ioBufferInput) {
        const char* aDescription;
        bool aSuccess = ioBufferInput.read(aDescription);
        return { aSuccess, aDescription };
    }

    static std::pair<Description, Description> getDescriptions() {
      return {
        Description("none", "Request message description"),
        Description("string**", "Give description of accepted message for current phase")
      };

    }

    using query = std::string;

    static void handle(::shd::network::tcp::Socket& ioSocket, const char* iDescription, const size_t iSize, shd::network::buffer::Output& ioBuffer) {
      size_t aTotalSize = sizeof(shd::network::message::Header) + iSize;
      ioBuffer.reset(shd::network::message::Header::id::description);
      ioBuffer.write(iDescription, iSize);
      ioSocket.write(ioBuffer, aTotalSize);
    }


  };

  struct Error {


      static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, shd::network::message::Header::id iErrorId, const char* iErrorString) {
          ioBuffer.reset(iErrorId);
          ioBuffer.write(iErrorString);
          return ioBuffer;
      }

      static std::tuple<bool, const char*> getResponse(shd::network::buffer::Input& ioBufferInput) {
          const char* aErrorString;
          bool aSuccess = ioBufferInput.read(aErrorString);
          return { aSuccess, aErrorString };
      }

      static std::pair<Description, Description> getDescriptions() {
          return {
            Description("",  ""),
            Description("uint8 + string", "Give error code and related message")
          };
      }
  };


  struct Acknowledge {

    static Description getDescription() {
      return Description("none", "Acknowledge message");
    }

    static std::pair<Description, Description> getDescriptions() {
      return {
        Description("",  ""),
        Description("uint8", "Acknowledge message from previous header code")
      };
    }
  };


  struct Rename {

    using query = char[16];


    static shd::network::buffer::Output& setQuery(shd::network::buffer::Output& ioBuffer, const char* iName) {
      ioBuffer.reset(shd::network::message::Header::id::rename);
      ioBuffer.write(iName);
      return ioBuffer;
    }

    static std::tuple<bool, const char*> getQuery(shd::network::buffer::Input& ioBufferInput) {
      const char* aName;
      bool aSuccess = ioBufferInput.read(aName);
      return { aSuccess, aName };
    }

    /*
    static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, int32_t iNum) {
      ioBuffer.reset(shd::network::message::Header::id::endianess);
      char buffer[20];
      snprintf(buffer, 12, "%d", iNum);
      ioBuffer.write(buffer);
      return ioBuffer;
    }

    static bool getResponse(shd::network::buffer::Input& ioBufferInput, const char*& ioIntCharPtr) {
      return ioBufferInput.read(ioIntCharPtr);
    }
    */

    static std::pair<Description, Description> getDescriptions() {
      return {
        Description("char[16]*             ", "Change your name on server"),
        Description("",  "")
      };
    }
  };


  struct Endianess {
    // bash check `lscpu`

    using query = int32_t;
    using response_t = char[12]; // -1234567890\0

    static shd::network::buffer::Output& setQuery(shd::network::buffer::Output& ioBuffer, int32_t iNum) {
      ioBuffer.reset(shd::network::message::Header::id::endianess);
      ioBuffer.write(iNum);
      return ioBuffer;
    }

    static bool getQuery(shd::network::buffer::Input& ioBufferInput, int32_t& ioInt) {
      return ioBufferInput.read(ioInt);
    }

    static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, int32_t iNum) {
      ioBuffer.reset(shd::network::message::Header::id::endianess);
      char buffer[20];
      snprintf(buffer, 12, "%d", iNum);
      ioBuffer.write(buffer);
      return ioBuffer;
    }

    static bool getResponse(shd::network::buffer::Input& ioBufferInput, const char*& ioIntCharPtr) {
      return ioBufferInput.read(ioIntCharPtr);
    }

    static std::pair<Description, Description> getDescriptions() {
      return {
        Description("int32_t",  "Send to server an int32 value"),
        Description("char[12]*", "Server will send back the input converted to string")
      };
    }
  };



  namespace chat {

    struct Text {

      static constexpr const size_t _kLength = 100;

      static shd::network::buffer::Output& setQuery(shd::network::buffer::Output& ioBuffer, const char* aMessage) {
        ioBuffer.reset(shd::network::message::Header::id::chat);
        ioBuffer.write(aMessage, std::min(strlen(aMessage), _kLength));
        return ioBuffer;
      }

      static std::tuple<bool, const char*> getQuery(shd::network::buffer::Input& ioBuffer) {
        const char* aResult;
        bool aSuccess = ioBuffer.read(aResult);
        return std::tuple<bool, const char*>{ aSuccess, aResult };
      }


      static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, shd::game::player::Name aName, const char* aMessage) {
        ioBuffer.reset(shd::network::message::Header::id::chat);
        ioBuffer.write(aName.data(), strlen(aName.data()));
        ioBuffer.write(aMessage, std::min(strlen(aMessage), _kLength));
        return ioBuffer;
      }

      static std::tuple<bool, const char*, const char*> getResponse(shd::network::buffer::Input& ioBuffer) {
        const char* aName;
        const char* aText;
        bool aSuccess = ioBuffer.read(aName);
        aSuccess &= ioBuffer.read(aText);
        return { aSuccess, aName, aText };
      }

      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("            char[101]*", "Send chat message to server phase"),
          Description("char[16]* + char[101]*", "Server will send chat message (101) from client (16)")
        };
      }


    };

    struct Whisper {

      static constexpr const size_t _kLength = 101;
#pragma pack(push, 1)
      struct query {
        char name[sizeof(network::message::Rename::query)];
        char message[_kLength];
      };
#pragma pack(pop)

      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("            char[101]*", "Send chat message to server phase"),
          Description("char[16]* + char[101]*", "Server will send chat message (101) from client (16)")
        };

      }
    };

  };



  namespace lobby {

    struct Change {
      using response = uint8_t;


      static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, uint8_t phaseId) {
          ioBuffer.reset(shd::network::message::Header::id::phaseChange);
          ioBuffer.write(phaseId);
          return ioBuffer;
      }

      static std::tuple<bool, uint8_t> getResponse(shd::network::buffer::Input& ioBuffer) {
          uint8_t aPhaseId; 
          bool aSuccess = ioBuffer.read(aPhaseId);
          return { aSuccess, aPhaseId };
      }

      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("",  ""),
          Description("uint8",  "Notify current lobby change when moved in (0: public, 1: private)")
        };
      }
    };


    struct Join {
      using query = ::shd::Key;


      static shd::network::buffer::Output& setQuery(shd::network::buffer::Output& ioBuffer, ::shd::Key iKey) {
          ioBuffer.reset(shd::network::message::Header::id::lobbyJoin);
          iKey.serialize(ioBuffer);
          return ioBuffer;
      }

      static std::tuple<bool, ::shd::Key> getQuery(shd::network::buffer::Input& ioBuffer) {
          ::shd::Key aKey;
          bool aSuccess = aKey.deserialize(ioBuffer);
          return std::tuple<bool, ::shd::Key>{ aSuccess, aKey };
      }

      static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, uint8_t iPhaseId) {
          return network::message::lobby::Change::setResponse(ioBuffer, iPhaseId);
      }

      static std::tuple<bool, uint8_t> getResponse(shd::network::buffer::Input& ioBuffer) {
          return network::message::lobby::Change::getResponse(ioBuffer);
      }

      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("char[X+1]* (X = <key size>)",  "Connect user to lobby that match the secret key"),
          Description("none",  "Lobby change or error LobbyNotFound")
        };
      }

    };

    struct Create {
      using response = ::shd::Key;

      static shd::network::buffer::Output& setQuery(shd::network::buffer::Output& ioBuffer) {
          ioBuffer.reset(shd::network::message::Header::id::lobbyCreate);
          return ioBuffer;
      }

      /*
      static std::tuple<bool, ::lobby::Key> getQuery(shd::network::buffer::Input& ioBuffer) {
          ::lobby::Key aKey;
          bool aSuccess = aKey.deserialize(ioBuffer);
          return std::tuple<bool, ::lobby::Key>{ aSuccess, aKey };
      }
      */

      static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, ::shd::Key iKey) {
          ioBuffer.reset(shd::network::message::Header::id::lobbyCreate);
          iKey.serialize(ioBuffer);
          return ioBuffer;
      }

      static std::tuple<bool, ::shd::Key> getResponse(shd::network::buffer::Input& ioBuffer) {
          ::shd::Key aKey;
          bool aSuccess = aKey.deserialize(ioBuffer);
          return { aSuccess, aKey };
      }

      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("none",  "Create a private lobby "),
          Description("char[X+1]* (X = <key size>)",  "The key needed to join the private lobby")
        };
      }

    };


    struct List {
      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("none",  "Request list all connected user"),
          Description("payload = X * char[16]*",  "All users given connected")
        };
      }

      static shd::network::buffer::Output& initResponse(shd::network::buffer::Output& ioBuffer) {
        ioBuffer.reset(shd::network::message::Header::id::lobbyList);
        return ioBuffer;
      }

      static shd::network::buffer::Output& addUserName(shd::network::buffer::Output& ioBuffer, const char* iUserName) {
        ioBuffer.write(iUserName);
        return ioBuffer;
      }

      static bool getResponse(shd::network::buffer::Input& ioBufferInput, std::vector<const char*>& oNameList) {
        oNameList.reserve(ioBufferInput.getHeader()->getPayloadSize() / 7);
        bool aSuccess = true;
        while (!ioBufferInput.finished()) {
          const char* aName;
          aSuccess &= ioBufferInput.read(aName);
          oNameList.push_back(aName);
        }
        return aSuccess;
      }

      static std::tuple<bool, std::vector<const char*>>  getResponse(shd::network::buffer::Input& ioBufferInput) {
        std::vector<const char*> aNameList;
        bool aSuccess = getResponse(ioBufferInput, aNameList);
        return std::tuple<bool, std::vector<const char*>>{ aSuccess, std::move(aNameList) };
      }



    };

    struct Quit {

      static shd::network::buffer::Output& setQuery(shd::network::buffer::Output& ioBuffer) {
          ioBuffer.reset(shd::network::message::Header::id::lobbyQuit);
          return ioBuffer;
      }

      static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, uint8_t iPhaseId) {
          return network::message::lobby::Change::setResponse(ioBuffer, iPhaseId);
      }

      static std::tuple<bool, uint8_t> getResponse(shd::network::buffer::Input& ioBuffer) {
          return network::message::lobby::Change::getResponse(ioBuffer);
      }

      /*
      static std::tuple<bool, const char*> getQuery(shd::network::buffer::Input& ioBuffer, const char* iPrefix) {
          const char* aResult;
          bool aSuccess = ioBuffer.read(aResult);
          return std::tuple<bool, const char*>{ aSuccess, aResult };
      }
      */



      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("none",  "Quit the current lobby, order: LobbyPriv -> LobbyPub -> disconnect"),
          Description("",  "")
        };
      }
    };

  }

  namespace game {


    struct Start {

#pragma pack(push, 1)
      struct query {
        ::shd::game::Configuration::adventureType adventureType;
      };
      struct response {
        uint32_t width;
        uint32_t height;
        ::shd::game::map::Position PlayerPosition[8];
        uint16_t stepSpeed;
      };
#pragma pack(pop)


      static shd::network::buffer::Output& setQuery(shd::network::buffer::Output& ioBuffer, ::shd::game::Configuration::adventureType iAdventureType) {
        ioBuffer.reset(shd::network::message::Header::id::gameStart);
        ioBuffer.write(iAdventureType);
        sizeof(response);
        return ioBuffer;
      }

      template<typename T>
      static std::tuple<bool, ::shd::game::Configuration::adventureType> getQuery(T& ioSocket, const char* iPrefix) {
        std::tuple<bool, ::game::Configuration::adventureType> aResult;
        std::get<bool>(aResult) = ioSocket.read(iPrefix, std::get<::game::Configuration::adventureType>(aResult));
        return aResult;
      }


      static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, const std::vector<::shd::game::player::Info>& iAllPlayer, const ::shd::game::Configuration& iConfig) {
        ioBuffer.reset(shd::network::message::Header::id::gameStart);
        //iDataGlobal.serialize(ioBuffer);
        ioBuffer.write(iConfig.getActionSpeedDefault());
        return ioBuffer;
      }

      template<typename T>
      static std::tuple<bool, ::shd::game::map::Size, std::vector<::shd::game::player::Info>&, uint16_t> getResponse(shd::network::buffer::Input& ioBuffer, ::shd::network::message::Header aHeader, T& ioSocket, const char* iPrefix) {
        std::tuple<bool, ::shd::game::map::Size, std::vector<::game::player::Info>&, uint16_t> aResult;
        ioBuffer.reset(shd::network::message::Header::id::gameStart);
        std::get<bool>(aResult) = ioSocket.read(iPrefix, ioBuffer.getPayload(), aHeader.getPayloadSize()).first;
        ioBuffer.read(std::get<::shd::game::map::Size   >(aResult).width, sizeof(::shd::game::map::Size::value_type));
        ioBuffer.read(std::get<::shd::game::map::Size   >(aResult).heigth, sizeof(::shd::game::map::Size::value_type));
        /*
        for (size_t i = 0; i < ::game:::kNbPlayerMax; i += 1) {
          std::get<::game::map::Position>(aResult).deserialize(ioBuffer);
        }
        */
        ioBuffer.read(std::get<uint16_t>(aResult));
        return aResult;
      }

      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("1 uint8",  "Start the game with the correponding mode [0:wander] "),
          Description("4 uint32 + 1 uint16",  "Give map width/height, player position x/y, and moveSpeed in milliseconds")
        };
      }

    };

    struct Stop {


      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("none",  "Stop the game ( /!\\ definitively) "),
          Description("none",  "The game stopped (as requested by user)")
        };
      }

    };



    struct playerMoveVector {

      static shd::network::buffer::Output& setQuery(shd::network::buffer::Output& ioBuffer, ::shd::math::Vector iPlayerMoveVector) {
        ioBuffer.reset(shd::network::message::Header::id::gamePlayerMoveVector);
        auto* header = ioBuffer.getHeader();

        iPlayerMoveVector.serialize(ioBuffer);
        header = ioBuffer.getHeader();
        return ioBuffer;
      }

      template<typename T>
      static std::tuple<bool, ::shd::math::Vector> getQuery(T& ioSocket, const char* iPrefix) {
        std::tuple<bool, math::Vector> aResult;

        math::Vector::type_t aDx, aDy;
        std::get<bool>(aResult) = ioSocket.read(iPrefix, reinterpret_cast<char*>(&aDx), sizeof(math::Vector::type_t)).first;
        std::get<bool>(aResult) &= ioSocket.read(iPrefix, reinterpret_cast<char*>(&aDy), sizeof(math::Vector::type_t)).first;
        std::get<math::Vector>(aResult).set(aDx, aDy);
        return aResult;
      }

      static shd::network::buffer::Output& setResponse(shd::network::buffer::Output& ioBuffer, uint8_t iPlayerNum, ::shd::game::map::Position iPlayerPosition, shd::math::Vector iPlayerMoveVector) {
        ioBuffer.reset(shd::network::message::Header::id::gamePlayerMoveVector);
        ioBuffer.write(iPlayerNum);
        iPlayerPosition.serialize(ioBuffer);
        iPlayerMoveVector.serialize(ioBuffer);
        return ioBuffer;
      }

      static size_t getResponseSize() {
        return sizeof(uint8_t) + 4 * sizeof(int32_t);
      }

      template<typename T>
      static std::tuple<bool, uint8_t, ::shd::game::map::Position, shd::math::Vector> getResponse(T& ioSocket, const char* iPrefix) {
        std::tuple<bool, uint8_t, ::shd::game::map::Position, shd::math::Vector> aResult;
        std::get<bool>(aResult) = ioSocket.read(iPrefix, reinterpret_cast<char*>(&std::get<uint8_t   >(aResult)), sizeof(uint8_t)).first;
        ::shd::game::map::Position::type_t aSubTypePos;
        std::get<bool>(aResult) &= ioSocket.read(iPrefix, reinterpret_cast<char*>(&aSubTypePos), sizeof(::shd::game::map::Position::type_t)).first;
        std::get<::shd::game::map::Position>(aResult).x = aSubTypePos;
        std::get<bool>(aResult) &= ioSocket.read(iPrefix, reinterpret_cast<char*>(&aSubTypePos), sizeof(::shd::game::map::Position::type_t)).first;
        std::get<::shd::game::map::Position>(aResult).y = aSubTypePos;

        math::Vector::type_t aDx, aDy;
        std::get<bool>(aResult) &= ioSocket.read(iPrefix, reinterpret_cast<char*>(&aDx), sizeof(shd::math::Vector::type_t)).first;
        std::get<bool>(aResult) &= ioSocket.read(iPrefix, reinterpret_cast<char*>(&aDy), sizeof(shd::math::Vector::type_t)).first;
        std::get<shd::math::Vector>(aResult).set(aDx, aDy);
        return aResult;
      }

      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("2 uint32",  "Indicate wished vector for player move (player will stop at end of vector)"),
          Description("uint8_t playerPos, 2*uint32 playerMapPosition, 2*uint32_t playerVector",  "Indicate the player position when receive vector")
        };
      }

    };

    struct playerMoveStop {
#pragma pack(push, 1)

      struct response {
        uint8_t playerNum;
        ::shd::game::map::Position PlayerPosition;
      };
#pragma pack(pop)

      static std::pair<Description, Description> getDescriptions() {
        return {
          Description("none",  "Indicate wished to stop action"),
          Description("2 uint32",  "Indicate the player position when receive vector")
        };
      }

    };



  };






};
