#pragma once


#include <cassert>
#include <vector>
#include <tuple>

#include "network/message/Header.h"

///#define LOG_BUFFER(VAR) LOG_VERBOSE("[buffer] " << VAR)
#define LOG_BUFFER(VAR) 

namespace network::buffer {

  using array_size_t = uint16_t;


  class Input {

    char* _buffer = new char[std::numeric_limits<uint16_t>::max()];
    uint16_t _posBuffer = 0;

    void clean() {
      LOG_BUFFER("Clean buffer " << this);
      if (_buffer) [[ likely ]] {
        LOG_BUFFER("Free allocation buffer : " << this << " - " << reinterpret_cast<size_t>(_buffer));
        delete[] _buffer;
        _buffer = nullptr;
      }
    }

    bool increasePosition(size_t iNum) {

      assert(getReadPosition() < getHeader()->getPayloadSize());
      _posBuffer += static_cast<uint16_t>(iNum);
      return getReadPosition() <= getHeader()->getPayloadSize();
    }

  public:
    Input() = default;

    Input(const Input& iBuffer) = delete;
    Input operator=(const Input& iBuffer) = delete;

    ~Input() {
      clean();
    }

    bool finished() const {
      return getReadPosition() == getHeader()->getPayloadSize();
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
      return &(_buffer[iPos]);
    }
    char* getPayloadEnd() {
      return getPayload(sizeof(network::message::Header) + getReadPosition());
    }

    char* getData() {
      return _buffer;
    }
    char* getData(const size_t iShift) {
      return &(_buffer[iShift]);
    }

    void reset() {
      _posBuffer = 0;
    }

    uint16_t getReadPosition() const {
      return _posBuffer;
    }

    void setData(const char* iData, const size_t iDataSize) {
      memcpy(getData(), iData, iDataSize);
      reset();
    }

#define READ(TYPE) bool read(TYPE& iItem) { memcpy(reinterpret_cast<char*>(&iItem), getPayloadEnd(), sizeof(TYPE)); return increasePosition(sizeof(TYPE)); }
    READ(uint8_t);
    READ(uint16_t);
    READ(uint32_t);
    READ(uint64_t);

    READ(int8_t);
    READ(int16_t);
    READ(int32_t);
    READ(int64_t);
#undef READ


    bool readSize(uint16_t& iItem) {
      return read(iItem);
    }

    bool read(char* iItem, const size_t iSizeMax) {
      uint16_t aSize; 
      if (!readSize(aSize)) {
        return false;
      }
      size_t aReadMax = std::min(static_cast<uint16_t>(iSizeMax), aSize);
      memcpy(iItem, getPayloadEnd(), aReadMax * sizeof(char));
      iItem[aReadMax -1 * sizeof(char)] = '\0';
      return increasePosition(aSize * sizeof(char));
    }

    bool read(const char*& oString) {
      uint16_t aSize;
      if (!readSize(aSize)) {
        return false;
      }
      getPayloadEnd()[aSize - 1] = '\0';
      oString = getPayloadEnd();
      return increasePosition(aSize);
    }

    template<typename T>
    void read(std::vector<T>& oTs) {
      uint16_t aVectorSize;
      read(aVectorSize);
      oTs.resize(aVectorSize);
      for (auto& iItem : oTs) {
        iItem.deserialize(*this);
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