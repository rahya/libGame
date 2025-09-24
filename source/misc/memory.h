#pragma once

#include <iostream>

#define DEBUG_PRINT(VAR) std::cout << __FUNCTION__ << " - " << #VAR << " : " << VAR << std::endl
#define DEBUG_STRING(VAR) printf("%s %d -> %.*s\n", __FUNCTION__, __LINE__, (int)SIZE_TO_BASIC(VAR.size()), VAR.__C())


class Memory {
public:
  static size_t& getAllocationCount() {
	static size_t kAllocationCount = 0;
	return kAllocationCount;
  }
  static size_t& getFreeCount() {
	static size_t kFreeCount = 0;
	return kFreeCount;
  }
};



#ifdef _WIN32 
  // debug_new.cpp
  // compile by using: cl /EHsc /W4 /D_DEBUG /MDd debug_new.cpp
  #define _CRTDBG_MAP_ALLOC
  #include <cstdlib>
  #include <crtdbg.h>


  //define DEBUG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
  #define DEBUG_NEW new 
  // Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
  // allocations to be of _CLIENT_BLOCK type
  #define MEMORY_LEAK_DETECTOR_BEGIN \
			  _CrtMemState sOld;\
			  _CrtMemState sNew;\
			  _CrtMemState sDiff;\
			  _CrtMemCheckpoint(&sOld); //take a snapchot


  #define MEMORY_LEAK_DETECTOR_END \
			  _CrtMemCheckpoint(&sNew); \
			  if (_CrtMemDifference(&sDiff, &sOld, &sNew)) {\
				  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);\
				  std::cout << "########################" << std::endl; \
				  std::cout << "# MEMORY LEAK DETECTED #" << std::endl; \
				  std::cout << "########################" << std::endl; \
				  std::cout << "\n##############" << std::endl; \
				  std::cout << "# Statistics #" << std::endl; \
				  std::cout << "##############" << std::endl; \
				  _CrtMemDumpStatistics(&sDiff);\
				  std::cout << "\n################" << std::endl; \
				  std::cout << "# Memory leaks #" << std::endl; \
				  std::cout << "################" << std::endl; \
				  _CrtDumpMemoryLeaks();\
			  }
		  /* May be useful !
				  std::cout << "\n###############" << std::endl; \
				  std::cout << "# All Objects #" << std::endl; \
				  std::cout << "###############" << std::endl; \
				  _CrtMemDumpAllObjectsSince(&sOld);\
		  */

#else
  #define DEBUG_NEW new
  #define MEMORY_LEAK_DETECTOR_BEGIN 
  #define MEMORY_LEAK_DETECTOR_END
  #define DEBUG_PRINT(VAR)  std::cout << __FUNCTION__ << " - " << #VAR << " : " << VAR << std::endl
#endif


namespace misc::memory {

  template<typename T>
  void set(char* ioBuffer, size_t& iPosition, const T& iItem) {
	memcpy(ioBuffer[iPosition], &iItem, sizeof(iItem));
	iPosition += sizeof(iItem);
  }


};

