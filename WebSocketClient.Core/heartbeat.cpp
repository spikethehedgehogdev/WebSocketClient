#include "heartbeat.h"

heartbeat::heartbeat(boost::asio::any_io_executor ex)
    : ping_(ex), watchdog_(ex)
{
}

void heartbeat::start(int ping_ms, int timeout_ms, ping_fn ping, timeout_fn timeout)
{
    stop();

    ping_ms_ = ping_ms;
    timeout_ms_ = timeout_ms;
    ping_cb_ = std::move(ping);
    timeout_cb_ = std::move(timeout);

    running_ = true;
    last_pong_ = std::chrono::steady_clock::now();

    schedule_ping();
    schedule_watchdog();
}

void heartbeat::stop()
{
    running_ = false;
    ping_.cancel();
    watchdog_.cancel();
}

void heartbeat::on_pong()
{
    last_pong_ = std::chrono::steady_clock::now();
}

void heartbeat::schedule_ping()
{
    if (!running_) return;

    ping_.expires_after(std::chrono::milliseconds(ping_ms_));
    ping_.async_wait([this](auto ec)
        {
            if (!ec && running_)
            {
                ping_cb_();
                schedule_ping();
            }
        });
}

void heartbeat::schedule_watchdog()
{
    if (!running_) return;

    watchdog_.expires_after(std::chrono::milliseconds(250));
    watchdog_.async_wait([this](auto ec)
        {
            if (ec || !running_) return;

            auto now = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - last_pong_).count();

            if (diff > timeout_ms_)
            {
                running_ = false;
                timeout_cb_();
                return;
            }

            schedule_watchdog();
        });
}