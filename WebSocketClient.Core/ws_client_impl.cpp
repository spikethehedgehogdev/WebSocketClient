#include "ws_client_impl.h"
#include "ws_url.h"

#include <boost/asio/steady_timer.hpp>

ws_client_impl::ws_client_impl(boost::asio::io_context& ioc)
    : ioc_(ioc)
    , connection_(ioc.get_executor())
    , heartbeat_(ioc.get_executor())
{
}

void ws_client_impl::set_callbacks(const ws_client_callbacks& cbs, void* user)
{
    dispatcher_.set(cbs, user);
}

void ws_client_impl::set_ping(int ping_ms, int pong_timeout_ms)
{
    ping_ms_ = ping_ms;
    pong_timeout_ms_ = pong_timeout_ms;
}

void ws_client_impl::connect(const std::string& url, const std::string& token)
{
    state_ = ws_state::connecting;

    manual_disconnect_ = false;
    std::string err;
    if (!parse_ws_url(url, url_, err))
    {
        dispatcher_.error(-1, err.c_str());
        return;
    }

    token_ = token;
    reconnect_.reset();
    start_connect();
}

void ws_client_impl::start_connect()
{
    heartbeat_.stop();

    connection_.connect(
        url_,
        token_,
        /* on_open */
        [this]()
        {
            state_ = ws_state::connected;

            reconnect_.reset();
            dispatcher_.connected(true);

            heartbeat_.start(
                ping_ms_,
                pong_timeout_ms_,
                [this]()
                {
                    dispatcher_.log("ping");
                    connection_.send_ping();
                },
                [this]()
                {
                    if (manual_disconnect_) return;

                    dispatcher_.log("pong timeout");
                    dispatcher_.timeout();
                    connection_.close();
                    schedule_reconnect();
                });
        },
        /* on_close */
        [this]()
        {
            state_ = ws_state::disconnected;

            dispatcher_.connected(false);
            if (!manual_disconnect_)
                schedule_reconnect();
        },
        /* on_data */
        [this](const char* d, int s, bool t)
        {
            dispatcher_.data(d, s, t);
        },
        /* on_error */
        [this](int c, const std::string& m)
        {
            dispatcher_.error(c, m.c_str());
        },
        /* on_pong */
        [this]()
        {
            heartbeat_.on_pong();
            dispatcher_.log("pong");
        });
}

void ws_client_impl::schedule_reconnect()
{
    state_ = ws_state::reconnecting;

    int delay = reconnect_.next_delay_ms();
    dispatcher_.reconnect(reconnect_.attempt(), delay);

    auto timer = std::make_shared<boost::asio::steady_timer>(ioc_);
    timer->expires_after(std::chrono::milliseconds(delay));
    timer->async_wait([this, timer](auto)
        {
            start_connect();
        });
}

void ws_client_impl::disconnect()
{
    manual_disconnect_ = true;
    heartbeat_.stop();
    connection_.close();
}


void ws_client_impl::send_text(const std::string& t)
{
    connection_.send_text(t);
}