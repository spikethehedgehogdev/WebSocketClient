#pragma once
#include <boost/asio.hpp>
#include <chrono>
#include <functional>

class heartbeat
{
public:
    using ping_fn = std::function<void()>;
    using timeout_fn = std::function<void()>;

    heartbeat(boost::asio::any_io_executor ex);

    void start(int ping_ms, int timeout_ms, ping_fn ping, timeout_fn timeout);
    void stop();
    void on_pong();

private:
    void schedule_ping();
    void schedule_watchdog();

    boost::asio::steady_timer ping_;
    boost::asio::steady_timer watchdog_;

    ping_fn ping_cb_;
    timeout_fn timeout_cb_;

    int ping_ms_ = 0;
    int timeout_ms_ = 0;
    bool running_ = false;

    std::chrono::steady_clock::time_point last_pong_;
};