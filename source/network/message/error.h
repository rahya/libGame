#pragma once

#include <array>
#include <string>
#include <cassert>


#include "network/buffer/Output.h"
#include "network/message/Header.h"
#include "network/message/io.h"


namespace network::message::error {


  using text = char[201];

  std::array<const char*, 7> getErrorStringArray() {
    std::array<const char*, 7> Error{
      "Server Full",
      "Lobby full",
      "Invalid read or message code",
      "Invalid String input (expect anything from ' ' to '~' included)",
      "Internal error (recv)",
      "Header only (message body missing)",
      "Lobby private not found",
    };
    return Error;
  };


  const char* getErrorString(const size_t iIndex) {
    assert(static_cast<size_t>(network::message::Header::id::__begin_error) < iIndex);
    assert(iIndex < static_cast<size_t>(network::message::Header::id::__last_error));
    assert(static_cast<size_t>(network::message::Header::id::__last_error) - static_cast<size_t>(network::message::Header::id::__begin_error) - 1 == getErrorStringArray().size());
    return getErrorStringArray()[iIndex - 100];
  }

  const char* getErrorString(const network::message::Header::id iType) {
    return getErrorString(static_cast<size_t>(iType));
  }

  size_t getPayloadSize() {
    return sizeof(text);
  }
  size_t getErrorSize() {
    return sizeof(network::message::Header) + sizeof(text);
  }


  std::pair<char*, size_t> create(const network::message::Header::id iType, text ioBuffer) {
    const char* aErrorText = getErrorString(iType);
    size_t aErrorTextSize = strlen(aErrorText)+1;

    network::message::Header aHeader{ iType, getPayloadSize() };
    memcpy(ioBuffer, reinterpret_cast<char*>(&aHeader), sizeof(network::message::Header));
    memcpy(&(ioBuffer[sizeof(network::message::Header)]), aErrorText, aErrorTextSize);
    return { ioBuffer, getErrorSize()};
  }

  network::buffer::Output& create(const network::message::Header::id iType, network::buffer::Output& ioBuffer) {
    const char* aErrorText = getErrorString(iType);
    size_t aErrorTextSize = strlen(aErrorText) + 1;
    ioBuffer.reset(iType);
    ioBuffer.write(aErrorText, aErrorTextSize);
    return ioBuffer;
  }





};
