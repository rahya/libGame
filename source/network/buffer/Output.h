#pragma once

#include <vector>
#include <cassert>
#include <tuple>

#include "network/message/Header.h"

///#define LOG_BUFFER(VAR) LOG_VERBOSE("[buffer] " << VAR)
#define LOG_BUFFER(VAR) 

namespace network::buffer {


  class Output {

    char* _buffer = new char[std::numeric_limits<uint16_t>::max()];

    void clean() {
      LOG_BUFFER("Clean buffer " << this);
      if (_buffer) [[ likely ]] {
        LOG_BUFFER("Free allocation buffer : " << this << " - " << reinterpret_cast<size_t>(_buffer));
        delete[] _buffer;
        _buffer = nullptr;
      }
    }

  public:
    Output() = default;

    Output(const Output& iBuffer) = delete;
    Output operator=(const Output& iBuffer) = delete;

    ~Output() {
      clean();
    }

    network::message::Header* getHeader() {
      return reinterpret_cast<network::message::Header*>(_buffer);
    }
    const network::message::Header* getHeader() const {
      return reinterpret_cast<network::message::Header*>(_buffer);
    }
    char* getPayload() {
      return &(_buffer[sizeof(network::message::Header)]);
    }
    char* getPayload(size_t iPos) {
      return &(_buffer[sizeof(network::message::Header) + iPos]);
    }
    char* getPayloadEnd() {
      return getPayload(getHeader()->getPayloadSize());
    }

    char* getData() {
      return _buffer;
    }
    char* getData(const size_t iShift) {
      return &(_buffer[iShift]);
    }

    void reset(const network::message::Header::id iCode) {
      getHeader()->setCode(iCode);
      getHeader()->payloadSize = 0;
    }

    size_t getStringOverhead() const {
      return 2;
    }

#define WRITE(TYPE) void write(TYPE iItem) { memcpy(getPayloadEnd(), &iItem, sizeof(TYPE)); getHeader()->setPayloadSize(getHeader()->getPayloadSize() + sizeof(TYPE)); }
    WRITE(uint8_t);
    WRITE(uint16_t);
    WRITE(uint32_t);
    WRITE(uint64_t);

    WRITE(int8_t);
    WRITE(int16_t);
    WRITE(int32_t);
    WRITE(int64_t);
#undef WRITE

    void write(const char* iItem, size_t iItemSize) {
      iItemSize += 1;
      write(static_cast<uint16_t>(iItemSize));
      memcpy(getPayloadEnd(), iItem, iItemSize);
      LOG_BUFFER("Add const char* : " << iItem << " -- Size : " << iItemSize << " -- Result : " << getPayloadEnd());
      getHeader()->setPayloadSize(getHeader()->getPayloadSize() + iItemSize);
    }
    void write(const std::string& iItem) {
      write(iItem.c_str(), iItem.size());
    }
    void write(const char* iItem) {
      write(iItem, strlen(iItem));
    }

    template<typename T>
    void write(const std::vector<T>& iTs) {
      write(static_cast<uint16_t>(iTs.size()));
      for (auto& iItem : iTs) {
        iItem.serialize(*this);
      }
    }


    operator char* () {
      return _buffer;
    }
    operator const char* () const {
      return _buffer;
    }

    bool valid() const {
      return _buffer != nullptr;
    }

  };

};

#undef LOG_BUFFER