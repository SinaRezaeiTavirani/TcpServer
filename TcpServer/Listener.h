#pragma once

#include <asio.hpp>
#include <asio/strand.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Session.h"

class Listener {
	asio::io_context& ioc_;
	tcp::acceptor acceptor_;

public:
	Listener(asio::io_context& ioc, tcp::endpoint endpoint);

	void run();

	std::function<void(int session_id, std::vector<char>&)> receiver_handler;
	void send_data(const int client_id, const std::vector<char>& data);

	void broad_cast(const std::vector<char>& data);



private:
	std::mutex session_list_mutex;

	void fail(asio::error_code ec, char const* what);

	void do_accept();

	void on_accept(asio::error_code ec, tcp::socket socket);

	std::atomic<int> id = 1000;
	std::unordered_map<int, std::unique_ptr<Session>> session_map;
};
