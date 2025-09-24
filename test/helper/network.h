#pragma once

#include "test/macro.h"

#include "network/message/Header.h"
#include "network/tcp/Socket.h"
#include "network/tcp/Client.h"
#include "network/tcp/Server.h"



namespace helper::network {


#define SMALL_SLEEP std::this_thread::sleep_for(std::chrono::milliseconds(10))

  inline void init(TEST_CAMPAIGN_PARAMETER, ::network::tcp::Server& oServer, std::function<void(::network::tcp::Socket&& iSocket)> aFunction) {

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

}