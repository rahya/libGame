#pragma once

#ifdef _WIN32
  #include <WinSock2.h>
  #pragma comment(lib,"WS2_32")
#endif

#include <iostream>
#include <string>

namespace network::tcp::error {


  using error_t = std::pair<int, std::string>;

  error_t get() {
#ifdef _WIN32
    int aErrorCode = WSAGetLastError();
    error_t aError{ aErrorCode, std::string{} };
    wchar_t* s = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, aErrorCode,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPWSTR)&s, 0, NULL);
    size_t aWCSSize = wcslen(s);
    aError.second.resize(aWCSSize);
    for (size_t i = 0; i < aWCSSize; i += 1) {
      aError.second[i] = s[i] & 255;
    }
    LocalFree(s);
    return aError;
#else
    return { errno, getErrnoString(errno) };
#endif
  }




};

std::ostream& operator<<(std::ostream& iOut, network::tcp::error::error_t aError) {
    iOut << aError.first << " " << aError.second;
    return iOut;
  }