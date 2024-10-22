#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "Listener.h"

using tcp = asio::ip::tcp;       

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr <<
            "Usage: server-async <address> <port> <threads>\n" <<
            "Example:\n" <<
            "    server-async 0.0.0.0 8080 1\n";
        return EXIT_FAILURE;
    }
    auto const address = asio::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const threads = std::max<int>(1, std::atoi(argv[3]));

    asio::io_context ioc{threads};

    std::make_shared<Listener>(
        ioc,
        tcp::endpoint{address, port})->run();

    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back(
            [&ioc]
            {
                ioc.run();
            });
    ioc.run();

    return EXIT_SUCCESS;
}