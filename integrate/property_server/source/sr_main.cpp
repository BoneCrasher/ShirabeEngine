/*!
 * @file      sr_main.cpp
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#include "sr_pch.h"

#include <functional>
#include <iostream>

#include "application/sr_state.h"
#include "application/sr_handler_factory.h"

#include <tcp_server_client/sr_server.h>

//<---------------------------------------------
int main( int aArgC, char* aArgV[] )
{
    if (aArgC != 2)
    {
      std::cerr << "Usage: ./property_server <port> \n";
      return 1;
    }

    std::string const port = aArgV[1];

    asio::io_context            asioIOContext;
    CStdSharedPtr_t<CTCPServer> tcpServer = nullptr;
    try
    {
        CStdSharedPtr_t<CTCPSession> session = nullptr;

        std::atomic<bool> connected(false);

        auto connectionSuccessHandler = [&] (CStdSharedPtr_t<CTCPSession> aSession)
        {
            connected.store(true);
            session = aSession;
        };

        auto const tcpThreadFn = [&](std::string const &aPort) -> int32_t
        {
            tcp::endpoint endPoint(tcp::v4(), std::atoi(aPort.c_str()));

            tcpServer = makeStdSharedPtr<CTCPServer>(asioIOContext, endPoint, connectionSuccessHandler);
            tcpServer->start();

            asioIOContext.run();

            return 0;
        };

        std::future<int32_t> tcpThreadResult = std::async(std::launch::async, tcpThreadFn, port);

        while(!connected.load())
        {
            std::cout << "Waiting for connection...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        CStdSharedPtr_t<CHandlerFactory> handlerFactory = makeStdSharedPtr<CHandlerFactory>(session);
        CStdSharedPtr_t<CState>          state          = makeStdSharedPtr<CState>(handlerFactory);

        std::string serializedState = std::string();

        bool const &stateInitialized = state->initialize();
        bool const serialized        = stateInitialized && state->getSerializedState(serializedState);

        CTCPMessage const outputMessage = CTCPMessage::create(serializedState);
        tcpServer->getSession()->writeMessage(outputMessage);

        std::string command = std::string();
        while(1)
        {
            std::cin >> command;

            if(!command.compare("exit"))
            {
                break;
            }

            if(!command.compare("write_property"))
            {
                std::string path{};
                (std::cin >> path);

                std::string index{};
                (std::cin >> index);

                std::string value{};
                (std::cin >> value);

                std::string outputCommand = StaticStringHelpers::formatString("writeProperty/%s/%s/%s", path.c_str(), index.c_str(), value.c_str());
                CTCPMessage outputMessage = CTCPMessage::create(outputCommand);

                tcpServer->getSession()->writeMessage(outputMessage);
            }
        }

        asioIOContext.stop();

        std::future_status status = std::future_status::ready;

        do
        {
            std::cout << "Waiting for TCP thread to return...\n";
            status = tcpThreadResult.wait_for(std::chrono::milliseconds(500));
        }
        while(std::future_status::ready != status);

        int32_t const tcpThreadReturned = tcpThreadResult.get();
        std::cout << "TCP thread returned status code: " << tcpThreadReturned << "\n";

        return tcpThreadReturned;

    }
    catch(std::exception const &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return -1;
    }
}
