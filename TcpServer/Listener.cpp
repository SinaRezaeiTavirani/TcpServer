#include "Listener.h"

Listener::Listener(asio::io_context& ioc, tcp::endpoint endpoint)
	: ioc_(ioc), acceptor_(asio::make_strand(ioc)) {
	asio::error_code ec;

	acceptor_.open(endpoint.protocol(), ec);
	if (ec) {
		fail(ec, "open");
		return;
	}

	acceptor_.set_option(asio::socket_base::reuse_address(true), ec);
	if (ec) {
		fail(ec, "set_option");
		return;
	}

	// Bind to the server address
	acceptor_.bind(endpoint, ec);
	if (ec) {
		fail(ec, "bind");
		return;
	}

	// Start listening for connections
	acceptor_.listen(asio::socket_base::max_listen_connections, ec);
	if (ec) {
		fail(ec, "listen");
		return;
	}
}

// Start accepting incoming connections
void Listener::run() { do_accept(); }

void Listener::fail(asio::error_code ec, char const* what) {
	std::cerr << what << ": " << ec.message() << "\n";
}

void Listener::do_accept() {
	// The new connection gets its own strand
	acceptor_.async_accept(asio::make_strand(ioc_),
		[this](asio::error_code ec, tcp::socket socket) {
			on_accept(ec, std::move(socket));
		});
}

void Listener::on_accept(asio::error_code ec, tcp::socket socket) {
	if (ec) {
		fail(ec, "accept");
		return; // To avoid infinite loop
	}
	else {
		std::cout << "Accepted ID: " << id
			<< " IP: " << socket.remote_endpoint().address()
			<< " Port: " << socket.remote_endpoint().port() << "\n";

		auto session = std::make_unique<Session>(std::move(socket), id);

		session->start();
		session->receiver_handler = receiver_handler;
		{
			std::lock_guard<std::mutex> lock(session_list_mutex);
			session_map[id] = std::move(session);
		}
		id++;
	}

	// Accept another connection
	do_accept();
}

void Listener::send_data(const int client_id, const std::vector<char>& data)
{
	if (session_map.find(client_id) != session_map.end())
	{
		session_map[client_id]->do_write(data);
	}
}

void Listener::broad_cast(const std::vector<char>& data)
{
	for (const auto& session : session_map)
	{
		session.second->do_write(data);
	}
}