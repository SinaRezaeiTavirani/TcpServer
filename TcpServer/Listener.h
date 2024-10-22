#pragma once

#include <asio.hpp>
#include <asio/strand.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "Session.h"
            



class Listener : public std::enable_shared_from_this<Listener>
{
    asio::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    Listener(
        asio::io_context& ioc,
        tcp::endpoint endpoint);

    // Start accepting incoming connections
    void
        run();

private:
    std::mutex session_list_mutex;

    void
        fail(asio::error_code ec, char const* what);

    void
        do_accept();

    void
        on_accept(asio::error_code ec, tcp::socket socket);

    std::atomic<int> id = 1000;
    std::vector<std::unique_ptr<Session>> alive_sessions;
};

