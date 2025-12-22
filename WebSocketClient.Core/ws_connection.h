#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>

#include <functional>
#include <memory>
#include <string>

#include "ws_url.h"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace ws = beast::websocket;
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp;

class ws_connection
{
public:
    using on_open_fn = std::function<void()>;
    using on_close_fn = std::function<void()>;
    using on_data_fn = std::function<void(const char*, int, bool)>;
    using on_error_fn = std::function<void(int, const std::string&)>;
    using on_pong_fn = std::function<void()>;

    ws_connection(asio::any_io_executor ex);

    void connect(
        const ws_url& url,
        const std::string& bearer,
        on_open_fn on_open,
        on_close_fn on_close,
        on_data_fn on_data,
        on_error_fn on_error,
        on_pong_fn on_pong);

    void send_ping();
    void send_text(const std::string& text);
    void close();
    bool is_open() const;

private:
    asio::strand<asio::any_io_executor> strand_;
    tcp::resolver resolver_;
    ssl::context ssl_ctx_;

    std::unique_ptr<ws::stream<beast::tcp_stream>> plain_;
    std::unique_ptr<ws::stream<beast::ssl_stream<beast::tcp_stream>>> secure_;

    beast::flat_buffer buffer_;

    ws_url url_;
    std::string bearer_;

    on_open_fn  on_open_;
    on_close_fn on_close_;
    on_data_fn  on_data_;
    on_error_fn on_error_;
    on_pong_fn  on_pong_;

    void do_resolve();
    void do_connect(tcp::resolver::results_type);
    void do_ssl_handshake();
    void do_ws_handshake();
    void do_read();

    template<class Fn>
    void with_ws(Fn&& fn);

    void fail(beast::error_code ec, const char* where);
};