#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <asio.hpp>

using asio::ip::tcp;

class Session
{
	asio::ip::tcp::socket socket_;

public:
	Session(asio::ip::tcp::socket socket, int id);
	~Session();
	void start();

	std::function<void(std::vector<char>)> receiver_handler;

private:
	void do_read();

	void do_write(const std::vector<char>& data);

	enum { max_length = 8192};
	bool is_alive_ = true;
	int id_;
};
