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

private:
	void do_read();

	void do_write(const std::vector<char>& data, std::size_t length);

	enum { max_length = 1024 };
	bool is_alive_ = true;
	int id_;
};
