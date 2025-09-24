
#include <cstdlib>


#include "test/networkMessage.h"
#include "test/networkTCP.h"
#include "test/networkTCPMessage.h"
#include "misc/memory.h"

void* operator new(std::size_t n)
{
  Memory::getAllocationCount() += 1;
  return malloc(n);
}
void* operator new[](std::size_t s)
{
  Memory::getAllocationCount() += 1;
  return malloc(s);
}

void operator delete(void* p)
{
  Memory::getFreeCount() += 1;
  return free(p);
}
void operator delete[](void* p)
{
  Memory::getFreeCount() += 1;
  return free(p);
}
void operator delete(void* p, std::size_t) {
  Memory::getFreeCount() += 1;
  return free(p);
}

int main() {

  MEMORY_LEAK_DETECTOR_BEGIN;
#ifdef _WIN32
  int wsaStatus; //check errors
  WSADATA WSAData;
  wsaStatus = WSAStartup(MAKEWORD(2, 0), &WSAData);
  if (wsaStatus != NO_ERROR) {
    LOG_ERROR("WSA Startup failed with error : " << wsaStatus);
    return EXIT_FAILURE;
  }
#endif

  {

    test::Campaign aCampaign;
    test::network::message::all(aCampaign);
    test::network::tcp::all(aCampaign);
    test::network::tcpMessage::all(aCampaign);
  }



#ifdef _WIN32
  WSACleanup();
#endif

  MEMORY_LEAK_DETECTOR_END;

  return EXIT_SUCCESS;
}