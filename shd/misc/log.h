#pragma once

#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <chrono>
#include <mutex>
#include <iostream>
#include <string.h>
#include <iomanip>

#include "shd/misc/Time.h"

namespace __ {


  class Log {

  private:

	std::ostream& _outStream;


	std::ostream& getOutStream() {
	  return _outStream;
	}

	std::ostream& date(std::ostream& ioOut)
	{
#define ADD_ZERO(X,NEXT) if((X) < 10){ ioOut << "0" << X  << NEXT; } else { ioOut << X  << NEXT; } 
	  ioOut << "[";
	  int y, m, d, h, mi, s;
	  Time::get(y, m, d, h, mi, s);
	  ADD_ZERO(y, "-");
	  ADD_ZERO(m, "-");
	  ADD_ZERO(d, " ");

	  ADD_ZERO(h, ":");
	  ADD_ZERO(mi, ":");
	  ADD_ZERO(s, "]");
	  return ioOut;
#undef ADD_ZERO
	}

  public:
	Log(std::ostream& iOutStream) : _outStream(iOutStream)	{}
	~Log() {};
	std::mutex _mutex;

	std::ostream& operator<<(auto iValue) {
	  return date(getOutStream()) << iValue;
	}
  };

  template<typename T>
  struct LogSub {
	public:
	  LogSub( T& iLogRef) : _logRef(iLogRef) {}
	  ~LogSub(){
		sstream << std::endl;
		_logRef << sstream.str();
	  }
	  T& _logRef;
	  std::stringstream sstream;

	  std::stringstream& operator<<(auto& iVal) {
		sstream << iVal;
		return sstream;
	  }

  };

}

namespace global {
  static __::Log log(std::cout);
};

inline std::string getErrnoString(int aErrno) {
#ifdef _WIN32
  char aBuffer[256];
  int aErrNo = strerror_s(aBuffer, errno);
  aBuffer[255] = '\0';
  return std::string{ aBuffer };
#else
  return strerror(errno);
#endif

}

#define __LOG_LOCK //std::lock_guard<std::mutex> lock(global::log->mutex())
#define __LOG_LOG(KEY,VAR) { std::lock_guard<std::mutex> lock(global::log._mutex); __::LogSub(global::log) << KEY << VAR; }

#define LOG_VERBOSE(VAR)  __LOG_LOG("[       ] ", VAR)
#define LOG_INFO(VAR)     __LOG_LOG("[ INFO  ] ", VAR)
#define LOG_WARNING(VAR)  __LOG_LOG("[WARNING] ", VAR)
#define LOG_ERROR(VAR)    __LOG_LOG("[ ERROR ] ", VAR)
#define LOG_FATAL(VAR)    __LOG_LOG("[  FATAL] ", VAR)


#define LOG_VAR(...)      LOG_VERBOSE(__VA_ARGS__)


#endif
