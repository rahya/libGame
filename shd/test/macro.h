#pragma once

#include <cassert>

#include "shd/misc/log.h"
#include "shd/misc/memory.h"

/*
    ~C_ ## NAME(){ LOG_INFO("End   test : " << #NAME << " ------------------------------------------------------"); }\
     C_ ## NAME(){ LOG_INFO("Begin test : " << #NAME << " ------------------------------------------------------"); }\
*/
namespace shd::test {

  struct TestData {
    size_t allocCount;
    size_t freeCount;
    size_t assertCount = 0;
  };

  struct Campaign {
    size_t testCount = 0;
    size_t assertCount = 0;

    size_t allocCount;
    size_t freeCount;

    std::chrono::high_resolution_clock::time_point testStart;
    std::chrono::high_resolution_clock::time_point campaignStart;

    TestData testData;

    Campaign() {
      allocCount = Memory::getAllocationCount();
      freeCount = Memory::getFreeCount();
    }

    ~Campaign() {

      const char* aSeparator = ", ";
      std::streamsize aShift = 8;
      std::cout << "\nCampaign : " << std::setw(aShift) << testCount << " tests"
        << aSeparator << std::setw(aShift) << assertCount << " asserts"
        << aSeparator << std::setw(aShift) << Memory::getFreeCount() - freeCount << " free"
        << aSeparator << std::setw(aShift) << Memory::getAllocationCount() - allocCount << " alloc"
        << std::endl;
    }

    void increaseAssertCount() {
      assertCount += 1;
    }

    void beginTest(const char* iName) {
      testData.allocCount = Memory::getAllocationCount();
      testData.freeCount = Memory::getFreeCount();
      testData.assertCount = assertCount;
      std::cout << std::setw(30) << iName << " : ongoing";
      fflush(stdout); 
      testStart = std::chrono::high_resolution_clock::now();

    }
    void endTest(const char* iName){

      auto t2 = std::chrono::high_resolution_clock::now();
      const char* aSeparator = "  |  ";

      std::streamsize aShift = 5;
      std::streamsize aShiftMemory = 4;
      std::streamsize aShiftTime = 3;
      size_t aAssertCount = assertCount - testData.assertCount;
      if (aAssertCount == 0) {
          std::cout << "\r" << std::setw(30) << iName << " : empty         " << std::endl;
      }
      else {
          auto ms_int = std::chrono::duration_cast<std::chrono::microseconds>(t2 - testStart).count();
          std::cout << "\r" << std::setw(30) << iName << " : done"
            << aSeparator <<"Asserts : " << std::setw(aShift) << aAssertCount
            << aSeparator <<"Time : " 
            << std::setw(aShiftTime) << ms_int/1000000          << "s "
            << std::setw(aShiftTime) << (ms_int%1000000 / 1000) << "ms "
            << std::setw(aShiftTime) << (ms_int % 1000)         << "us "
            << aSeparator << "Memory : " << std::setw(aShiftMemory) << Memory::getFreeCount() - testData.freeCount << " / " << std::setw(aShiftMemory) << Memory::getAllocationCount() - testData.allocCount
            << std::endl;
      }
    }
  };

};

#define TEST_CREATE(NAME) \
  struct C_ ## NAME{\
    ::shd::test::Campaign& iCampaign;\
    C_ ## NAME(::shd::test::Campaign& ioCampaign) : iCampaign(ioCampaign) { iCampaign.beginTest(#NAME); }\
    ~C_ ## NAME() { iCampaign.endTest(#NAME); }\
    void test();\
  };\
\
void C_ ## NAME::test()

#define TEST_CALL(NAME) { \
     C_ ## NAME a ## NAME(iCampaign); a ## NAME.test();  iCampaign.testCount +=1;\
}

#define ASSERT_EQ(A,B) if((A)!=(B)){ std::cout << std::endl << "Invalid test : " << #A << " {" << A << "} != {" << B << "} " << #B << std::endl;  } assert(A == B); iCampaign.increaseAssertCount()
#define ASSERT_TRUE(A)  { bool aTest = A; if(!aTest){ std::cout << std::endl << "Invalid test : " << #A << " {" << aTest << "}" << std::endl;  } assert( aTest);} iCampaign.increaseAssertCount()
#define ASSERT_FALSE(A) { bool aTest = A; if( aTest){ std::cout << std::endl << "Invalid test : " << #A << " {" << aTest << "}" << std::endl;  } assert(!aTest);} iCampaign.increaseAssertCount()


#define ASSERT_EQ_STR(A,B) if(strlen(A)!=strlen(B) || strcmp(A,B) != 0){ std::cout << std::endl << "Invalid test : " << #A << " {" << A << "} != {" << B << "} " << #B << std::endl;  } iCampaign.increaseAssertCount()



#define TEST_CAMPAIGN ::shd::test::Campaign aCampaign;
#define TEST_CAMPAIGN_ARGUMENT iCampaign
#define TEST_CAMPAIGN_PARAMETER ::shd::test::Campaign& iCampaign

#define TEST_CAMPAIGN_SUB(NAME) std::cout << "\n    Test : " << NAME << std::endl;

