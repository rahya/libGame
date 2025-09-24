#pragma once

#include "test/macro.h"

#include "network/message/Header.h"
#include "network/message/all.h"


TEST_CREATE(Endianess) {

  auto test = [](test::Campaign& iCampaign, network::buffer::Output& ioBufferOutput, network::buffer::Input& ioBufferInput, int32_t iInt) {
    ::network::message::Endianess::setQuery(ioBufferOutput, iInt);
    ioBufferInput.setData(ioBufferOutput.getData(), ioBufferOutput.getHeader()->getPayloadSize() + sizeof(network::message::Header));

    ASSERT_EQ(ioBufferInput.getHeader()->getCode(), network::message::Header::id::endianess);
    ASSERT_EQ(ioBufferInput.getHeader()->getPayloadSize(), 4);
    int aValue;
    network::message::Endianess::getQuery(ioBufferInput, aValue);
    ASSERT_TRUE(ioBufferInput.finished());
    ASSERT_EQ(aValue, iInt);


    ::network::message::Endianess::setResponse(ioBufferOutput, iInt);
    ioBufferInput.setData(ioBufferOutput.getData(), ioBufferOutput.getHeader()->getPayloadSize() + sizeof(network::message::Header));
    ASSERT_EQ(ioBufferInput.getHeader()->getCode(), network::message::Header::id::endianess);
    std::string aIntString = std::to_string(iInt);
    ASSERT_EQ(ioBufferInput.getHeader()->getPayloadSize(), sizeof(network::buffer::array_size_t) + aIntString.size() + 1);

    const char* aIntCharPtr;
    network::message::Endianess::getResponse(ioBufferInput, aIntCharPtr);
    ASSERT_TRUE(ioBufferInput.finished());
    ASSERT_EQ(aIntString, aIntCharPtr);
    };

  network::buffer::Output aBufferOutput;
  network::buffer::Input aBufferInput;
  std::vector<int> aVector = { 0, 1, 10, 123456789, std::numeric_limits<int32_t>::max(), -1, -10, -123456789, std::numeric_limits<int32_t>::min() };
  for (auto aInt : aVector) {
    test(iCampaign, aBufferOutput, aBufferInput, aInt);
  }
}

TEST_CREATE(text_chat) {

  auto test = [](test::Campaign& iCampaign, network::buffer::Output& ioBufferOutput, network::buffer::Input& ioBufferInput, const char* iText, const char* iExpectedResult) {

    ::network::message::chat::Text::setQuery(ioBufferOutput, iText);
    ASSERT_EQ(ioBufferOutput.getHeader()->getCode(), network::message::Header::id::chat);
    ASSERT_EQ(ioBufferOutput.getHeader()->getPayloadSize(), ioBufferOutput.getStringOverhead() + strlen(iExpectedResult) + 1 );
    ioBufferInput.setData(ioBufferOutput.getData(), sizeof(network::message::Header) + ioBufferOutput.getHeader()->getPayloadSize());
    ASSERT_EQ(ioBufferInput.getHeader()->getCode(), network::message::Header::id::chat);
    ASSERT_EQ(ioBufferInput.getHeader()->getPayloadSize(), ioBufferOutput.getStringOverhead() + strlen(iExpectedResult) + 1);
    auto [aSuccess, aText] = network::message::chat::Text::getQuery(ioBufferInput);
    ASSERT_TRUE(ioBufferInput.finished());
    ASSERT_TRUE(aSuccess);
    ASSERT_EQ_STR(aText, iExpectedResult);
    const char* aName = "test_name";
    ioBufferOutput.reset(network::message::Header::id::chat);
    ::network::message::chat::Text::setResponse(ioBufferOutput, aName, iText);
    
    ioBufferInput.setData(ioBufferOutput.getData(), sizeof(network::message::Header) + ioBufferOutput.getHeader()->getPayloadSize());

    auto [aSuccessResponse, aName0, aMessage1] = ::network::message::chat::Text::getResponse(ioBufferInput);
    ASSERT_TRUE(aSuccessResponse);
    ASSERT_EQ_STR(aName, aName0);
    ASSERT_EQ_STR(aMessage1, iExpectedResult);
  };

  std::vector<const char*> aVecText = { "a", "ab", "abc", "Jambon", "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" };

  network::buffer::Output aBufferOutput;
  network::buffer::Input aBufferInput;
  for (const char*& aText : aVecText) {
    test(iCampaign, aBufferOutput, aBufferInput, aText, aText);
  }

  std::string aTooLongString = aVecText.back();
  aTooLongString += '1';
  test(iCampaign, aBufferOutput, aBufferInput, aTooLongString.c_str(), aVecText.back());


}

TEST_CREATE(rename_user) {
  auto test = [](test::Campaign& iCampaign, network::buffer::Output& ioBufferOutput, network::buffer::Input& ioBufferInput, const char* iName, const char* iExpectedResult) {
    ::network::message::Rename::setQuery(ioBufferOutput, iName);
    ASSERT_EQ(ioBufferOutput.getHeader()->getCode(), network::message::Header::id::rename);
    ASSERT_EQ(ioBufferOutput.getHeader()->getPayloadSize(), ioBufferOutput.getStringOverhead() + strlen(iExpectedResult) + 1);
    ioBufferInput.setData(ioBufferOutput.getData(), sizeof(network::message::Header) + ioBufferOutput.getHeader()->getPayloadSize());

    ASSERT_EQ(ioBufferInput.getHeader()->getCode(), network::message::Header::id::rename);
    ASSERT_EQ(ioBufferInput.getHeader()->getPayloadSize(), ioBufferOutput.getStringOverhead() + strlen(iExpectedResult) + 1);
    auto [aSuccess, aName] = network::message::Rename::getQuery(ioBufferInput);
    ASSERT_TRUE(ioBufferInput.finished());
    ASSERT_TRUE(aSuccess);
    ASSERT_EQ_STR(aName, iExpectedResult);
    };

  std::vector<const char*> aVecText = { "a", "ab", "abc", "Jambon", "zirkonya", "123456789012345"};

  network::buffer::Output aBufferOutput;
  network::buffer::Input aBufferInput;
  for (const char*& aText : aVecText) {
    test(iCampaign, aBufferOutput, aBufferInput, aText, aText);
  }
}

TEST_CREATE(Get_description) {

    network::buffer::Output aBufferOutput;
    network::buffer::Input aBufferInput;
    network::message::Description::setQuery(aBufferOutput);
    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());
    ASSERT_EQ(aBufferInput.getHeader()->getCode(), network::message::Header::id::description);
    ASSERT_EQ(aBufferInput.getHeader()->getPayloadSize(), 0);

    const char* aString = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
    network::message::Description::setResponse(aBufferOutput, aString);
    ASSERT_EQ(aBufferOutput.getHeader()->getCode(), network::message::Header::id::description);
    ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), aBufferOutput.getStringOverhead() + strlen(aString) + 1);

    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());

    auto [aSuccess, aDescription] = network::message::Description::getResponse(aBufferInput);
    ASSERT_TRUE(aSuccess);
    ASSERT_EQ_STR(aDescription, aString);


}

TEST_CREATE(Get_error) {

    network::buffer::Output aBufferOutput;
    network::buffer::Input aBufferInput;

    for (size_t i = static_cast<size_t>(network::message::Header::id::__begin_error) + 1; i < static_cast<size_t>(network::message::Header::id::__last_error); i += 1) {
        network::message::Header::id aErrorId = static_cast<network::message::Header::id>(i);
        network::message::Error::setResponse(aBufferOutput, aErrorId, network::message::error::getErrorString(aErrorId));
        ASSERT_EQ(aBufferOutput.getHeader()->getCode(), aErrorId);
        ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), aBufferOutput.getStringOverhead() + strlen(network::message::error::getErrorString(aErrorId)) + 1);

        aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());

        auto [aSuccess, aErrorString] = network::message::Description::getResponse(aBufferInput);
        ASSERT_TRUE(aSuccess);
        ASSERT_EQ_STR(aErrorString, network::message::error::getErrorString(aErrorId));
    }
}


TEST_CREATE(Change_lobby) {

    network::buffer::Output aBufferOutput;
    network::buffer::Input aBufferInput;

    for (size_t i = 0; i < std::numeric_limits<uint8_t>::max(); i += 1) {
        uint8_t aPhaseIdServer = static_cast<uint8_t>(i);
        network::message::lobby::Change::setResponse(aBufferOutput, aPhaseIdServer);
        ASSERT_EQ(aBufferOutput.getHeader()->getCode(), network::message::Header::id::phaseChange);
        ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), sizeof(aPhaseIdServer));

        aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());

        auto [aSuccess, aPhaseIdClient] = network::message::lobby::Change::getResponse(aBufferInput);
        ASSERT_TRUE(aSuccess);
        ASSERT_EQ(aPhaseIdClient, aPhaseIdServer);
    } 
}

TEST_CREATE(Create_lobby) {

    network::buffer::Output aBufferOutput;
    network::buffer::Input aBufferInput;
    network::message::lobby::Create::setQuery(aBufferOutput);
    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());
    ASSERT_EQ(aBufferInput.getHeader()->getCode(), network::message::Header::id::lobbyCreate);
    ASSERT_EQ(aBufferInput.getHeader()->getPayloadSize(), 0);

    lobby::Key aKeyServer;
    network::message::lobby::Create::setResponse(aBufferOutput, aKeyServer);
    ASSERT_EQ(aBufferOutput.getHeader()->getCode(), network::message::Header::id::lobbyCreate);
    ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), aBufferOutput.getStringOverhead() + strlen(aKeyServer.get()) + 1);


    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());
    auto [aSuccess, aKeyClient] = network::message::lobby::Create::getResponse(aBufferInput);
    ASSERT_TRUE(aSuccess);
    ASSERT_EQ(aKeyClient, aKeyServer);


}

TEST_CREATE(Join_lobby) {

    network::buffer::Output aBufferOutput;
    network::buffer::Input aBufferInput;
    lobby::Key aKeyGameClient;
    network::message::lobby::Join::setQuery(aBufferOutput, aKeyGameClient);

    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());

    ASSERT_EQ(aBufferInput.getHeader()->getCode(), network::message::Header::id::lobbyJoin);
    ASSERT_EQ(aBufferInput.getHeader()->getPayloadSize(), aBufferOutput.getStringOverhead() + strlen(aKeyGameClient.get()) + 1);

    auto [aSuccessServer, aKeyGameServer] = network::message::lobby::Join::getQuery(aBufferInput);
    ASSERT_TRUE(aSuccessServer);
    ASSERT_EQ(aKeyGameServer, aKeyGameClient);


    network::message::lobby::Join::setResponse(aBufferOutput, 1);
    ASSERT_EQ(aBufferOutput.getHeader()->getCode(), network::message::Header::id::phaseChange);
    ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), sizeof(uint8_t));

    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());

    auto [aSuccessClient, aPhaseId] = network::message::lobby::Join::getResponse(aBufferInput);
    ASSERT_TRUE(aSuccessClient);
    ASSERT_EQ(aPhaseId, 1);


}


TEST_CREATE(Quit_lobby) {
    network::buffer::Output aBufferOutput;
    network::buffer::Input aBufferInput;
    network::message::lobby::Quit::setQuery(aBufferOutput);
    ASSERT_EQ(aBufferOutput.getHeader()->getCode(), network::message::Header::id::lobbyQuit);
    ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), 0);

    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());

    ASSERT_EQ(aBufferInput.getHeader()->getCode(), network::message::Header::id::lobbyQuit);
    ASSERT_EQ(aBufferInput.getHeader()->getPayloadSize(), 0);


    network::message::lobby::Quit::setResponse(aBufferOutput, 0);
    ASSERT_EQ(aBufferOutput.getHeader()->getCode(), network::message::Header::id::phaseChange);
    ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), sizeof(uint8_t));

    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());

    auto [aSuccessClient, aPhaseId] = network::message::lobby::Quit::getResponse(aBufferInput);
    ASSERT_TRUE(aSuccessClient);
    ASSERT_EQ(aPhaseId, 0);

}


TEST_CREATE(UserList_lobby) {

  std::vector<const char*> aVecText = { "a", "ab", "abc", "Jambon", "zirkonya", "123456789012345" };

  network::buffer::Output aBufferOutput;
  network::buffer::Input aBufferInput;
  network::message::lobby::List::initResponse(aBufferOutput);
  for (const char*& aText : aVecText) {
    network::message::lobby::List::addUserName(aBufferOutput, aText);
  }

  aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());
  
  auto [aSuccess, aNameList] = network::message::lobby::List::getResponse(aBufferInput);
  ASSERT_EQ(aNameList.size(), aVecText.size());
  for (size_t i = 0; i < std::min(aNameList.size(), aVecText.size()); i += 1) {
    ASSERT_EQ_STR(aNameList[i], aVecText[i]);
  }
  

}
TEST_CREATE(start_Game) {}
TEST_CREATE(stop_Game) {}
TEST_CREATE(start_UserMove_Game) {}
TEST_CREATE(stop_UserMove_Game) {}


TEST_CREATE(rename_chat_0) {

    network::buffer::Output aBufferOutput;
    network::buffer::Input aBufferInput;

    //client
    const char* aNameClient = "Killing Eve";
    ::network::message::Rename::setQuery(aBufferOutput, aNameClient);
    ASSERT_EQ(aBufferOutput.getHeader()->getCode(), network::message::Header::id::rename);
    ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), aBufferOutput.getStringOverhead() + strlen(aNameClient) + 1);
    
    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());
    //server
    ASSERT_EQ(aBufferInput.getHeader()->getCode(), network::message::Header::id::rename);
    ASSERT_EQ(aBufferInput.getHeader()->getPayloadSize(), aBufferOutput.getStringOverhead() + strlen(aNameClient) + 1);
    auto [aSuccess, aNameServer] = network::message::Rename::getQuery(aBufferInput);
    ASSERT_TRUE(aBufferInput.finished());
    ASSERT_TRUE(aSuccess);
    ASSERT_EQ_STR(aNameServer, aNameClient);
    std::string aPersistentNameServer = aNameServer;

    //client
    const char* aTextClient = "Lorem ipsum";
    ::network::message::chat::Text::setQuery(aBufferOutput, aTextClient);
    ASSERT_EQ(aBufferOutput.getHeader()->getCode(), network::message::Header::id::chat);
    ASSERT_EQ(aBufferOutput.getHeader()->getPayloadSize(), aBufferOutput.getStringOverhead() + strlen(aTextClient) + 1);

    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());
    //server
    ASSERT_EQ(aBufferInput.getHeader()->getCode(), network::message::Header::id::chat);
    ASSERT_EQ(aBufferInput.getHeader()->getPayloadSize(), aBufferOutput.getStringOverhead() + strlen(aTextClient) + 1);
    auto [aSuccess0, aTextServer] = network::message::chat::Text::getQuery(aBufferInput);
    ASSERT_TRUE(aBufferInput.finished());
    ASSERT_TRUE(aSuccess0);
    ASSERT_EQ_STR(aTextServer, aTextClient);

    aBufferOutput.reset(network::message::Header::id::chat);
    ::network::message::chat::Text::setResponse(aBufferOutput, aPersistentNameServer.c_str(), aTextServer);
    ASSERT_EQ(aPersistentNameServer, aNameClient);

    aBufferInput.setData(aBufferOutput.getData(), sizeof(network::message::Header) + aBufferOutput.getHeader()->getPayloadSize());
    //client
    auto [aSuccess1, aName0, aMessage1] = ::network::message::chat::Text::getResponse(aBufferInput);
    ASSERT_TRUE(aSuccess1);
    ASSERT_EQ_STR(aNameClient, aName0);
    ASSERT_EQ_STR(aMessage1, aTextClient);



}



namespace test::network::message {

  void all(TEST_CAMPAIGN_PARAMETER) {
    TEST_CAMPAIGN_SUB("network::message");
    TEST_CALL(Endianess);
    TEST_CALL(text_chat);
    TEST_CALL(rename_user);
    TEST_CALL(Get_description);
    TEST_CALL(Get_error);
    TEST_CALL(Change_lobby);
    TEST_CALL(Create_lobby);
    TEST_CALL(Join_lobby);
    TEST_CALL(UserList_lobby);
    TEST_CALL(Quit_lobby);
    TEST_CALL(start_Game);
    TEST_CALL(stop_Game);
    TEST_CALL(start_UserMove_Game);
    TEST_CALL(stop_UserMove_Game);

    TEST_CALL(rename_chat_0);


  }

};




