#pragma once

#include <boost/asio.hpp>
#include <string>
#include <chrono>
#include <atomic>

#include "ws_connection.h"
#include "event_dispatcher.h"
#include "reconnect_policy.h"
#include "heartbeat.h"

class ws_client_impl
{
public:
    ws_client_impl(boost::asio::io_context& ioc);

    void set_callbacks(const ws_client_callbacks& cbs, void* user);

    void connect(const std::string& url, const std::string& token);
    void disconnect();
    void send_text(const std::string&);

    void set_ping(int ping_ms, int pong_timeout_ms);

private:
    boost::asio::io_context& ioc_;

    ws_connection connection_;
    event_dispatcher dispatcher_;
    reconnect_policy reconnect_;
    heartbeat heartbeat_;

    std::atomic_bool manual_disconnect_{ false };

    ws_url url_;
    std::string token_;

    int ping_ms_ = 5000;
    int pong_timeout_ms_ = 12000;

    enum class ws_state
    {
        disconnected,
        connecting,
        connected,
        reconnecting
    };

    ws_state state_ = ws_state::disconnected;

    void start_connect();
    void schedule_reconnect();
};