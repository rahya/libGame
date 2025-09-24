#pragma once

#include <array>
#include <functional>

#include "network/message/all.h"
#include "network/message/error.h"


namespace network::message {

  struct Manager {

    std::function<void(::game::player::Info*, network::message::Header)> _functionDefault;

    Manager(std::function<void(::game::player::Info*, network::message::Header)> iFunctionDefault) {
      std::get<1>(_descFun[0]) = iFunctionDefault;
      for (size_t i = 2; i < _descFun.size(); i += 1) {
        std::get<1>(_descFun[i]) = iFunctionDefault;
      }
      _functionDefault = iFunctionDefault;
    }


    size_t size() const {
      return _descFun.size();
    }

    std::function<void(::game::player::Info*, network::message::Header)> operator[](const size_t iPos) {
      return std::get<1>(_descFun[iPos]);
    }

    using messageDescFun = std::tuple<
      std::pair<network::message::Description, network::message::Description>,
      std::function<void(::game::player::Info*, network::message::Header)>,
      const char*,
      network::message::Header::id
    >;
    std::array<messageDescFun, 14> _descFun = {
#define NEW_MESSAGE(NAME,ID) messageDescFun{ network::message::NAME         ::getDescriptions(), nullptr, #NAME, ID }
      NEW_MESSAGE(Endianess,                    network::message::Header::id::endianess),
      NEW_MESSAGE(Description,                  network::message::Header::id::description),
      NEW_MESSAGE(Rename,                       network::message::Header::id::rename),
      NEW_MESSAGE(chat::Text,                   network::message::Header::id::chat),
      NEW_MESSAGE(lobby::Create,                network::message::Header::id::lobbyCreate),
      NEW_MESSAGE(lobby::Join,                  network::message::Header::id::lobbyJoin),
      NEW_MESSAGE(lobby::List,                  network::message::Header::id::lobbyList),
      NEW_MESSAGE(lobby::Quit,                  network::message::Header::id::lobbyQuit),
      NEW_MESSAGE(lobby::Change,                network::message::Header::id::phaseChange),
      NEW_MESSAGE(Acknowledge,                  network::message::Header::id::acknowledege),
      NEW_MESSAGE(game::Start,                  network::message::Header::id::gameStart),
      NEW_MESSAGE(game::Stop,                   network::message::Header::id::gameStop),
      NEW_MESSAGE(game::playerMoveVector,   network::message::Header::id::gamePlayerMoveVector),
      NEW_MESSAGE(game::playerMoveStop,     network::message::Header::id::gamePlayerMoveStop),

    };
#undef NEW_MESSAGE


    const std::string& getDescription() {
      if (_description.empty()) {
        size_t aSizeMax = 0;
        size_t aNameMax = 0;
        for (auto& [aDesc, aFun, aName, aId] : _descFun) {
          if (aSizeMax < strlen(aDesc.first.inputDetails)) { aSizeMax = strlen(aDesc.first.inputDetails); }
          if (aSizeMax < strlen(aDesc.second.inputDetails)) { aSizeMax = strlen(aDesc.second.inputDetails); }
          if (aNameMax < strlen(aName)) {
            aNameMax = strlen(aName);
          }
        }
        std::ostringstream oss;
        oss << "Communication Data Structure (Header + message) :\n";
        oss << "            1          2             \n";
        oss << " 01234567 89012345 67890123 ...      \n";
        oss << "+--------+--------+--------+------------\n";
        oss << "|  Code  |   size payload  | payload ...\n";
        oss << "+--------+--------+--------+------------\n";

        oss << "\nMessage :\n";
        auto setDesc = [aSizeMax, aNameMax](std::ostringstream& oss, const size_t aSize, const char* aWay, const network::message::Description& aDesc, const char* iName) {
          oss << std::setw(aNameMax) << iName << " | " << std::setw(6) << aSize << " | " << aWay << " | " << std::setw(aSizeMax) << aDesc.inputDetails << " | " << aDesc.processDetails << "\n";
          };

        oss << std::setw(aNameMax) << " " << " | " << "header |        |        | " << std::setw(aSizeMax) << "" << " | \n";
        oss << std::setw(aNameMax) << "Name" << " | " << "code   | source | dest   | " << std::setw(aSizeMax) << "Payload info" << " | Description\n";
        oss << std::setw(aNameMax) << "----------------" << "-+-" << "-------+--------+--------+-" << std::setw(aSizeMax) << "---------------------------" << "-+-------------------------------------------------\n";
        //for (size_t i = 0; i < _descFun.size(); i += 1) {
        for (auto& [aDesc, aFun, aName, aId] : _descFun) {
          int i = static_cast<int>(aId);
          if (std::get<1>(_descFun[i]).target_type() == _functionDefault.target_type()) {
            continue;
          }
          if (strlen(std::get<0>(_descFun[i]).first.inputDetails)) { setDesc(oss, i, "client | server", std::get<0>(_descFun[i]).first, std::get<2>(_descFun[i])); }
          if (strlen(std::get<0>(_descFun[i]).second.inputDetails)) { setDesc(oss, i, "server | client", std::get<0>(_descFun[i]).second, std::get<2>(_descFun[i])); }
        }
        oss << "\n  * : string[] or string must have a trailing '\\0' as part of allocated space, char[16] = char[15] + '\\0')\n";
        oss << " ** : string size is defined by payload size (included trailing '\\0')\n";

        oss << "\n\nError (server -> client): \n";
        oss << " code  | " << std::setw(15) << "Payload info" << " | Description\n";
        oss << "-------+-" << std::setw(15) << "---------------" << "-+-------------------------------------------------\n";
        for (size_t i = static_cast<size_t>(network::message::Header::id::ServerFull); i < static_cast<size_t>(network::message::Header::id::__last_error); i += 1) {
          oss << std::setw(6) << i << " | " << std::setw(15) << "string" << " | " << network::message::error::getErrorString(i) << "\n";
        }
        _description = oss.str();
      }
      return _description;
    }

    void update(Header::id iId, std::function<void(::game::player::Info*, network::message::Header)> iFunction) {
      std::get<1>(_descFun[static_cast<int>(iId)]) = iFunction;
    }

    std::string _description;

  };

};