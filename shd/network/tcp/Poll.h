#pragma once


#include <fcntl.h>
#ifndef _WIN32
#include <poll.h>
#endif

#include "Socket.h"

namespace shd::network::tcp {

  class Poll {

	std::vector<struct pollfd> _pollFdSet;


	template<typename T>
	unsigned long socketSet(std::vector<T>& iVecSocket) {
	  unsigned long i = 0;
	  _pollFdSet.resize(iVecSocket.size());
	  for (T& aType : iVecSocket) {
		if (aType.getSocket().isValid() == false) {
		  continue;
		}
		_pollFdSet[i].fd = aType.getSocket().get();
		_pollFdSet[i].events = POLLIN;
		i += 1;
	  }
	  return i;
	}

	
	/*
	fd_set* getFdSet() {
	  return reinterpret_cast<fd_set*>(_fdSet);
	}
	*/

  public:



	template<typename T>
	std::pair<bool, std::vector<T*>> select(std::vector<T>& iVecSocket, int iTimeout = -1) {
	  
	  unsigned long aNbSocket = socketSet(iVecSocket);
	  //int selectReady = ::select(static_cast<int>(aSocketMax + 1), getFdSet(), nullptr, nullptr, iTimeout);
#ifdef _WIN32
	  int selectReady = ::WSAPoll(_pollFdSet.data(), aNbSocket, iTimeout);
#else
	  int selectReady = ::poll(_pollFdSet.data(), aNbSocket, iTimeout);
#endif
	  if (selectReady == -1) {
		LOG_ERROR("Failed poll : " << network::tcp::error::get());
		return { false , {} };
	  }
	  if (selectReady == 0) {
		return { true , {} };
	  }
	  std::vector<T*> aVecSocket;
	  aVecSocket.reserve(selectReady);
	  {
		size_t i = -1;
		for (T& aType : iVecSocket) {
		  i+=1;
		  if (aType.getSocket().isValid() == false) {
			continue;
		  }
		  if (_pollFdSet[i].revents) {
			aVecSocket.push_back(&aType);
		  }
		}
	  }

	  return { true, aVecSocket };
	}


  };


};