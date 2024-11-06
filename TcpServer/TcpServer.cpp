#include "Listener.h"
#include <iostream>
#include <memory>
#include <string>
#include <thread>


using tcp = asio::ip::tcp;

int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "Usage: TcpServer.exe <address> <port> <threads>\n"
			<< "Example:\n"
			<< "    TcpServer 0.0.0.0 8080 1\n";
		return EXIT_FAILURE;
	}
	auto const address = asio::ip::make_address(argv[1]);
	auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
	auto const threads = std::max<int>(1, std::atoi(argv[3]));

	asio::io_context ioc{threads};

	std::shared_ptr<Listener> listener =
		std::make_shared<Listener>(ioc, tcp::endpoint{address, port});

	listener->run();
	listener->receiver_handler = [listener](int session_id, std::vector<char>& data, std::size_t length) {
		std::string result(length, ' ');
		std::copy_n(data.data(), std::min(data.size(), length), result.begin());

		std::cout << result << std::endl;
	};

	std::vector<std::thread> v;
	v.reserve(threads - 1);
	for (auto i = threads - 1; i > 0; --i)
		v.emplace_back([&ioc] { ioc.run(); });
	ioc.run();

	return EXIT_SUCCESS;
}