#include "Session.h"


Session::Session(asio::ip::tcp::socket socket, int id)
	: socket_(std::move(socket)), id_(id), is_alive_(true)
{
}

Session::~Session()
{
	if (socket_.is_open())
		socket_.close();
}

void Session::start()
{
	do_read();
}

void Session::do_read()
{
	auto v_buffer = std::make_shared<std::vector<char>>(8192);
	socket_.async_read_some(asio::buffer(v_buffer->data(), v_buffer->size()),
		[this, v_buffer](asio::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				std::string data(v_buffer->begin(), v_buffer->end());
				std::cout << data;
				std::string received_ack = "I've receive your message\n";
				const std::vector<char> rec_ack_vec(received_ack.begin(), received_ack.end());
				do_write(rec_ack_vec,length);
			}
			else {
				std::cout << ec.message() << std::endl;
				if(socket_.is_open())
					socket_.close();
				is_alive_ = false;
			}
		});
}

void Session::do_write(const std::vector<char>& data, std::size_t length)
{
	
	socket_.async_write_some(asio::buffer(data.data(), data.size()),
		[this](asio::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{

				std::cout << "data wrote\n";
				do_read();
			}
			else {
				std::cout << "Error: " << ec.message() << std::endl;
			}
		});
}