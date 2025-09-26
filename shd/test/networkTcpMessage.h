#pragma once
#pragma once

#include "shd/test/macro.h"

#include "shd/network/message/Header.h"
#include "shd/network/tcp/Socket.h"
#include "shd/network/tcp/Client.h"
#include "shd/network/tcp/Server.h"

#include "shd/test/helper/network.h"


TEST_CREATE(endianess_clientServer) {

  shd::network::tcp::Server aServer;

  std::vector<shd::network::tcp::Socket> aSocketServerSide;
  auto aSocketExporter = [&aSocketServerSide](shd::network::tcp::Socket&& iSocket) {
    aSocketServerSide.push_back(std::move(iSocket));
   };
  init(TEST_CAMPAIGN_ARGUMENT, aServer, aSocketExporter);

  shd::network::tcp::Client aClient;
  init(TEST_CAMPAIGN_ARGUMENT, aClient);
  SMALL_SLEEP;
  ASSERT_EQ(aSocketServerSide.size(), 1);
  ASSERT_TRUE(aSocketServerSide[0].isValid());


  auto test = [](shd::test::Campaign& iCampaign, shd::network::buffer::Output& ioBufferOutput, shd::network::buffer::Input& ioBufferInput, shd::network::tcp::Socket& aSocketClientOnServerSide, shd::network::tcp::Client& aClient, int32_t iInt) {
    ::shd::network::message::Endianess::setQuery(ioBufferOutput, iInt);
    aClient.write(ioBufferOutput);

    aSocketClientOnServerSide.read(__FUNCTION__, ioBufferInput);
    int aValue;
    bool aSuccess = shd::network::message::Endianess::getQuery(ioBufferInput, aValue);
    ASSERT_TRUE(aSuccess);
    ASSERT_TRUE(ioBufferInput.finished());
    ASSERT_EQ(aValue, iInt);

    ::shd::network::message::Endianess::setResponse(ioBufferOutput, iInt);
    aSocketClientOnServerSide.write(ioBufferOutput);
    std::string aIntString = std::to_string(iInt);

    const char* aIntCharPtr;
    aClient.read(__FUNCTION__, ioBufferInput);
    shd::network::message::Endianess::getResponse(ioBufferInput, aIntCharPtr);
    ASSERT_TRUE(ioBufferInput.finished());
    ASSERT_EQ(aIntString, aIntCharPtr);
    };

  shd::network::buffer::Output aBufferOutput;
  shd::network::buffer::Input aBufferInput;
  std::vector<int> aVector = { 0, 1, 10, 123456789, std::numeric_limits<int32_t>::max(), -1, -10, -123456789, std::numeric_limits<int32_t>::min() };
  for (auto aInt : aVector) {
    test(iCampaign, aBufferOutput, aBufferInput, aSocketServerSide[0], aClient, aInt);
  }

  close(TEST_CAMPAIGN_ARGUMENT, aServer);

}
TEST_CREATE(renameUser_clientServer) {
  shd::network::tcp::Server aServer;

  std::vector<shd::network::tcp::Socket> aSocketServerSide;
  auto aSocketExporter = [&aSocketServerSide](shd::network::tcp::Socket&& iSocket) {
    aSocketServerSide.push_back(std::move(iSocket));
    };
  init(TEST_CAMPAIGN_ARGUMENT, aServer, aSocketExporter);

  shd::network::tcp::Client aClient;
  init(TEST_CAMPAIGN_ARGUMENT, aClient);
  SMALL_SLEEP;
  ASSERT_EQ(aSocketServerSide.size(), 1);
  ASSERT_TRUE(aSocketServerSide[0].isValid());

  auto test = [](shd::test::Campaign& iCampaign, shd::network::buffer::Output& ioBufferOutput, shd::network::buffer::Input& ioBufferInput, 
    shd::network::tcp::Socket& aSocketClientOnServerSide, shd::network::tcp::Client& aClient, const char* iName, const char* iExpectedResult) {
    ::shd::network::message::Rename::setQuery(ioBufferOutput, iName);
    ASSERT_TRUE(aClient.write(ioBufferOutput));

    aSocketClientOnServerSide.read(__FUNCTION__, ioBufferInput);
    auto [aSuccess, aName] = shd::network::message::Rename::getQuery(ioBufferInput);
    ASSERT_TRUE(aSuccess);
    ASSERT_EQ_STR(aName, iExpectedResult);
    };

  std::vector<const char*> aVecText = { "a", "ab", "abc", "Jambon", "zirkonya", "123456789012345" };

  shd::network::buffer::Output aBufferOutput;
  shd::network::buffer::Input aBufferInput;
  for (const char*& aText : aVecText) {
    test(iCampaign, aBufferOutput, aBufferInput, aSocketServerSide[0], aClient, aText, aText);
  }

  close(TEST_CAMPAIGN_ARGUMENT, aServer);
}

TEST_CREATE(textChat_clientServer) {
  shd::network::tcp::Server aServer;

  std::vector<shd::network::tcp::Socket> aSocketServerSide;
  auto aSocketExporter = [&aSocketServerSide](shd::network::tcp::Socket&& iSocket) {
    aSocketServerSide.push_back(std::move(iSocket));
    };
  init(TEST_CAMPAIGN_ARGUMENT, aServer, aSocketExporter);

  shd::network::tcp::Client aClient;
  init(TEST_CAMPAIGN_ARGUMENT, aClient);
  SMALL_SLEEP;
  ASSERT_EQ(aSocketServerSide.size(), 1);
  ASSERT_TRUE(aSocketServerSide[0].isValid());

  auto test = [](shd::test::Campaign& iCampaign, shd::network::buffer::Output& ioBufferOutput, shd::network::buffer::Input& ioBufferInput, 
    shd::network::tcp::Socket& aSocketClientOnServerSide, shd::network::tcp::Client& aClient, const char* iText, const char* iExpectedResult) {

    ::shd::network::message::chat::Text::setQuery(ioBufferOutput, iText);
    ASSERT_TRUE(aClient.write(ioBufferOutput));

    aSocketClientOnServerSide.read(__FUNCTION__, ioBufferInput);
    auto [aSuccess, aText] = shd::network::message::chat::Text::getQuery(ioBufferInput);
    ASSERT_TRUE(ioBufferInput.finished());
    ASSERT_TRUE(aSuccess);
    ASSERT_EQ_STR(aText, iExpectedResult);

    const char* aName = "test_name";
    ::shd::network::message::chat::Text::setResponse(ioBufferOutput, aName, iText);
    ASSERT_TRUE(aSocketClientOnServerSide.write(ioBufferOutput));

    aClient.read(__FUNCTION__, ioBufferInput);
    auto [aSuccessResponse, aName0, aMessage1] = ::shd::network::message::chat::Text::getResponse(ioBufferInput);
    ASSERT_TRUE(aSuccessResponse);
    ASSERT_EQ_STR(aName, aName0);
    ASSERT_EQ_STR(aMessage1, iExpectedResult);
    };

  std::vector<const char*> aVecText = { "a", "ab", "abc", "Jambon", "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" };

  shd::network::buffer::Output aBufferOutput;
  shd::network::buffer::Input aBufferInput;
  for (const char*& aText : aVecText) {
    test(iCampaign, aBufferOutput, aBufferInput, aSocketServerSide[0], aClient, aText, aText);
  }

  std::string aTooLongString = aVecText.back();
  aTooLongString += '1';
  test(iCampaign, aBufferOutput, aBufferInput, aSocketServerSide[0], aClient, aTooLongString.c_str(), aVecText.back());


  close(TEST_CAMPAIGN_ARGUMENT, aServer);
}

namespace shd::test::network::tcpMessage {

  void all(TEST_CAMPAIGN_PARAMETER) {
    TEST_CAMPAIGN_SUB("shd::network::tcpMessage");
    TEST_CALL(endianess_clientServer);
    TEST_CALL(renameUser_clientServer);
    TEST_CALL(textChat_clientServer);


  }

};