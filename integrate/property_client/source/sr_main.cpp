/*!
 * @file      sr_main.cpp
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#include "sr_pch.h"

#include <functional>
#include <iostream>
#include <mutex>

#include <core/kernel/sr_kernel.h>
#include <core/base/types/smartptr/sr_cppstd_ptr.h>

#include <tcp_server_client/sr_client.h>

int main( int aArgC, char* aArgV[] )
{
    if (aArgC != 3)
    {
      std::cerr << "Usage: ./property_server <host> <port> \n";
      return 1;
    }

    std::string const host = aArgV[1];
    std::string const port = aArgV[2];

    try
    {
        asio::io_context            asioIOContext;
        CStdSharedPtr_t<CTCPClient> tcpClient = nullptr;

        auto const tcpThreadFn = [&](
                std::string const &aHost,
                std::string const &aPort) -> int32_t
        {
            tcp::resolver    resolver(asioIOContext);
            auto endpoints = resolver.resolve(aHost, aPort);

            if(endpoints.empty())
            {
                std::cout << "No endpoints discovered...\n";
                return -1;
            }

            tcpClient = makeStdSharedPtr<CTCPClient>(asioIOContext, endpoints);
            tcpClient->start();

            asioIOContext.run();

            tcpClient->stop();

            return 0;
        };

        std::future<int32_t> tcpThreadResult = std::async(std::launch::async, tcpThreadFn, host, port);

        std::string command{};
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

                std::string outputCommand = CString::formatString("writeProperty/%s/%s/%s", path.c_str(), index.c_str(), value.c_str());
                CTCPMessage outputMessage = CTCPMessage::create(outputCommand);

                tcpClient->getSession()->writeMessage(outputMessage);
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
