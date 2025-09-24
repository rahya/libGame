#pragma once

#include "test/macro.h"

#include "network/message/Header.h"
#include "network/tcp/Socket.h"
#include "network/tcp/Client.h"
#include "network/tcp/Server.h"


TEST_CREATE(Header) {
  network::message::Header aHeader;
  aHeader.set(network::message::Header::id::endianess, 10);
  ASSERT_EQ(network::message::Header::id::endianess, aHeader.getCode());
  ASSERT_EQ(10ULL, aHeader.getPayloadSize());
}

void writeBuffer(TEST_CAMPAIGN_PARAMETER, network::buffer::Output& ioBuffer){
  ASSERT_TRUE(ioBuffer.valid());

  ioBuffer.reset(network::message::Header::id::description);
  ASSERT_EQ(ioBuffer.getHeader()->getCode(), network::message::Header::id::description);
  ASSERT_EQ(ioBuffer.getHeader()->getPayloadSize(), 0ULL);

#define INSERT(TYPE,VALUE) { \
  size_t aPayloadSize = ioBuffer.getHeader()->getPayloadSize(); \
  ioBuffer.write(static_cast<TYPE>(VALUE)); \
  ASSERT_EQ(aPayloadSize + sizeof(TYPE), ioBuffer.getHeader()->getPayloadSize());\
}

  INSERT(uint8_t, 0);
  INSERT(uint16_t, 1);
  INSERT(uint32_t, 2);
  INSERT(uint64_t, 3);


  INSERT(int8_t, 4);
  INSERT(int16_t, 5);
  INSERT(int32_t, 6);
  INSERT(int64_t, 7);
#undef INSERT


  size_t aIntWriteSize = 30;
  ASSERT_EQ(ioBuffer.getHeader()->getPayloadSize(), aIntWriteSize);
  const char* aStringTest = "A string test !";
  ioBuffer.write(aStringTest, strlen(aStringTest));
  ASSERT_EQ(ioBuffer.getHeader()->getPayloadSize(), aIntWriteSize + sizeof(uint16_t) + strlen(aStringTest) + 1);
  ioBuffer.write(aStringTest, strlen(aStringTest));

}


void readBuffer(TEST_CAMPAIGN_PARAMETER, network::buffer::Input& ioBuffer) {

#define READ(TYPE,VALUE) { \
  size_t aPayloadSize = ioBuffer.getReadPosition(); \
  TYPE aValue = 0;\
  ioBuffer.read(aValue); \
  ASSERT_EQ(aPayloadSize + sizeof(TYPE), ioBuffer.getReadPosition());\
  ASSERT_EQ(static_cast<uint64_t>(aValue), VALUE);\
}
  READ(uint8_t, 0);
  READ(uint16_t, 1);
  READ(uint32_t, 2);
  READ(uint64_t, 3);


  READ(int8_t, 4);
  READ(int16_t, 5);
  READ(int32_t, 6);
  READ(int64_t, 7);
#undef READ

  constexpr const size_t aSizeTemp = 200;
  char aTemp[aSizeTemp];
  ioBuffer.read(aTemp, aSizeTemp);

  const char* aStringTest = "A string test !";
  ASSERT_EQ_STR(aTemp, aStringTest);

  const char* aString;
  ASSERT_TRUE(ioBuffer.read(aString));
  ASSERT_EQ_STR(aString, aStringTest);

  ASSERT_EQ(ioBuffer.getHeader()->getPayloadSize(), ioBuffer.getReadPosition());
}

TEST_CREATE(Buffer) {
  network::buffer::Output aBuffer;
  writeBuffer(TEST_CAMPAIGN_ARGUMENT, aBuffer);
 
  network::buffer::Input aBufferInput;
  aBufferInput.setData(aBuffer.getData(), sizeof(network::message::Header) + aBuffer.getHeader()->getPayloadSize() );
  ASSERT_EQ(aBuffer.getHeader()->getPayloadSize(), aBufferInput.getHeader()->getPayloadSize());

  readBuffer(TEST_CAMPAIGN_ARGUMENT, aBufferInput);

}



  struct aStruct {
    int32_t aValue = rand();
    void serialize(network::buffer::Output& ioBuffer) const {
      ioBuffer.write(aValue);
    }
    void deserialize(network::buffer::Input& ioBuffer) {
      ioBuffer.read(aValue);
    }
    bool operator==(const aStruct& iStruct) const {
      return iStruct.aValue == aValue;
    }
    friend std::ostream& operator<<(std::ostream& oStream, const aStruct& iStruct) {
      oStream << iStruct.aValue;
      return oStream;
    }
  };
TEST_CREATE(BufferVector) {

  std::vector<aStruct> aAllStruct0;
  aAllStruct0.resize(10);


  network::buffer::Output aBuffer;
  aBuffer.reset(network::message::Header::id::description);
  aBuffer.write(aAllStruct0);


  network::buffer::Input aBufferInput;
  aBufferInput.setData(aBuffer.getData(), aBuffer.getHeader()->getPayloadSize() + sizeof(network::message::Header));


  std::vector<aStruct> aAllStruct1;
  aBufferInput.read(aAllStruct1);
  ASSERT_EQ(aAllStruct0.size(), aAllStruct1.size());
  for (size_t i = 0; i < aAllStruct0.size(); i += 1) {
    ASSERT_EQ(aAllStruct0[i], aAllStruct1[i]);
  }
  

}

TEST_CREATE(Socket) {

  network::tcp::Socket aSocket;
  ASSERT_FALSE(aSocket.isValid());
  ASSERT_TRUE(aSocket.isInvalid());

  aSocket.init();
  ASSERT_TRUE(aSocket.isValid());
  ASSERT_FALSE(aSocket.isInvalid());

  aSocket.close();
  ASSERT_FALSE(aSocket.isValid());
  ASSERT_TRUE(aSocket.isInvalid());


}

#define SMALL_SLEEP std::this_thread::sleep_for(std::chrono::milliseconds(10))

inline void init(TEST_CAMPAIGN_PARAMETER, ::network::tcp::Server& oServer, std::function<void(network::tcp::Socket&& iSocket)> aFunction) {

  oServer.setPort(1234);
  oServer.getSocket().init();
  oServer.getSocket().setAddrLoopBack();
  oServer.setSocketExporter(aFunction);
  oServer.init();

  oServer.run();
  SMALL_SLEEP;

  ASSERT_TRUE(oServer.isRunning());

}


inline void close(TEST_CAMPAIGN_PARAMETER, ::network::tcp::Server& oServer) {
  oServer.setClosing();

  ASSERT_FALSE(oServer.isRunning());

  oServer.join();
  ASSERT_TRUE(oServer.isClosed());
}

inline void init(TEST_CAMPAIGN_PARAMETER, ::network::tcp::Client& oClient) {

  oClient.setAddr("127.0.0.1");
  oClient.setPort(1234);
  oClient.init();
  ASSERT_TRUE(oClient.connect());
  
}


TEST_CREATE(connect_ClientServer) {

  network::tcp::Server aServer;

  network::tcp::Socket aSocketServerSide;
  auto aSocketExporter = [&aSocketServerSide](network::tcp::Socket&& iSocket){
    aSocketServerSide = std::move(iSocket);
  };
  init(TEST_CAMPAIGN_ARGUMENT, aServer, aSocketExporter);

  network::tcp::Client aClient;
  ASSERT_TRUE(aSocketServerSide.isInvalid());
  init(TEST_CAMPAIGN_ARGUMENT, aClient);
  SMALL_SLEEP;
  ASSERT_TRUE(aSocketServerSide.isValid());


  close(TEST_CAMPAIGN_ARGUMENT, aServer);

}

TEST_CREATE(exchange_ClientServer) {

  network::tcp::Server aServer;

  network::tcp::Socket aSocketServerSide;
  auto aSocketExporter = [&aSocketServerSide](network::tcp::Socket&& iSocket) {
    aSocketServerSide = std::move(iSocket);
    };
  init(TEST_CAMPAIGN_ARGUMENT, aServer, aSocketExporter);

  network::tcp::Client aClient;
  ASSERT_TRUE(aSocketServerSide.isInvalid());
  init(TEST_CAMPAIGN_ARGUMENT, aClient);
  SMALL_SLEEP;
  ASSERT_TRUE(aSocketServerSide.isValid());

  network::buffer::Output aBufferOutput;
  writeBuffer(TEST_CAMPAIGN_ARGUMENT, aBufferOutput);
  aClient.write(aBufferOutput);

  network::buffer::Input aBufferInput;
  aSocketServerSide.read(__FUNCTION__, aBufferInput);
  ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), aBufferInput.getHeader()->getPayloadSize());
  readBuffer(TEST_CAMPAIGN_ARGUMENT, aBufferInput);



  close(TEST_CAMPAIGN_ARGUMENT, aServer);

}


TEST_CREATE(twoClient_ClientServer) {

  network::tcp::Server aServer;

  std::vector<network::tcp::Socket> aSocketServerSide;
  auto aSocketExporter = [&aSocketServerSide](network::tcp::Socket&& iSocket) {
    aSocketServerSide.push_back(std::move(iSocket));
    };
  init(TEST_CAMPAIGN_ARGUMENT, aServer, aSocketExporter);

  ASSERT_EQ(aSocketServerSide.size(), 0);
  network::tcp::Client aClient0;
  init(TEST_CAMPAIGN_ARGUMENT, aClient0);
  SMALL_SLEEP;
  network::tcp::Client aClient1;
  init(TEST_CAMPAIGN_ARGUMENT, aClient1);
  SMALL_SLEEP;
  ASSERT_TRUE(aSocketServerSide[0].isValid());
  ASSERT_TRUE(aSocketServerSide[1].isValid());

  network::buffer::Output aBufferOutput;
  writeBuffer(TEST_CAMPAIGN_ARGUMENT, aBufferOutput);
  aClient0.write(aBufferOutput);
  aClient1.write(aBufferOutput);

  network::buffer::Input aBufferInput;
  aSocketServerSide[0].read(__FUNCTION__, aBufferInput);
  ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), aBufferInput.getHeader()->getPayloadSize());
  readBuffer(TEST_CAMPAIGN_ARGUMENT, aBufferInput);

  aBufferInput.reset();
  aSocketServerSide[1].read(__FUNCTION__, aBufferInput);
  ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), aBufferInput.getHeader()->getPayloadSize());
  readBuffer(TEST_CAMPAIGN_ARGUMENT, aBufferInput);


  close(TEST_CAMPAIGN_ARGUMENT, aServer);

}


namespace test::network::tcp {

  void all(TEST_CAMPAIGN_PARAMETER) {
    TEST_CAMPAIGN_SUB("network::tcp");
    TEST_CALL(Header);
    TEST_CALL(Buffer);
    TEST_CALL(BufferVector);
    TEST_CALL(Socket);
    TEST_CALL(  connect_ClientServer);
    TEST_CALL( exchange_ClientServer);
    TEST_CALL(twoClient_ClientServer);


  }

};