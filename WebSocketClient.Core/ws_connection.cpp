#include "ws_connection.h"

#include <openssl/ssl.h>

ws_connection::ws_connection(asio::any_io_executor ex)
    : strand_(asio::make_strand(ex))
    , resolver_(strand_)
    , ssl_ctx_(ssl::context::tls_client)
{
    ssl_ctx_.set_default_verify_paths();
    ssl_ctx_.set_verify_mode(ssl::verify_none);
}

bool ws_connection::is_open() const
{
    if (plain_)  return plain_->is_open();
    if (secure_) return secure_->is_open();
    return false;
}

void ws_connection::connect(
    const ws_url& url,
    const std::string& bearer,
    on_open_fn on_open,
    on_close_fn on_close,
    on_data_fn on_data,
    on_error_fn on_error,
    on_pong_fn on_pong)
{
    url_ = url;
    bearer_ = bearer;
    on_open_ = on_open;
    on_close_ = on_close;
    on_data_ = on_data;
    on_error_ = on_error;
    on_pong_ = on_pong;

    asio::post(strand_, [this]() { do_resolve(); });
}

void ws_connection::do_resolve()
{
    resolver_.async_resolve(
        url_.host,
        url_.port,
        [this](beast::error_code ec, tcp::resolver::results_type res)
        {
            if (ec) return fail(ec, "resolve");

            if (url_.secure)
                secure_ = std::make_unique<ws::stream<beast::ssl_stream<beast::tcp_stream>>>(strand_, ssl_ctx_);
            else
                plain_ = std::make_unique<ws::stream<beast::tcp_stream>>(strand_);

            do_connect(res);
        });
}

void ws_connection::do_connect(tcp::resolver::results_type res)
{
    with_ws([this, res](auto& w)
        {
            beast::get_lowest_layer(w).async_connect(
                res,
                [this](beast::error_code ec, const tcp::endpoint&)
                {
                    if (ec) return fail(ec, "connect");
                    if (url_.secure) do_ssl_handshake();
                    else do_ws_handshake();
                });
        });
}

void ws_connection::do_ssl_handshake()
{
    if (!secure_) return;

    if (!SSL_set_tlsext_host_name(
        secure_->next_layer().native_handle(),
        url_.host.c_str()))
    {
        fail({}, "sni");
        return;
    }

    secure_->next_layer().async_handshake(
        ssl::stream_base::client,
        [this](beast::error_code ec)
        {
            if (ec) return fail(ec, "ssl handshake");
            do_ws_handshake();
        });
}

void ws_connection::do_ws_handshake()
{
    const std::string host = make_host_header(url_);
    const std::string bearer = bearer_;

    with_ws([this, host, bearer](auto& w)
        {
            w.set_option(ws::stream_base::timeout::suggested(beast::role_type::client));
            w.control_callback([this](ws::frame_type t, beast::string_view)
                {
                    if (t == ws::frame_type::pong && on_pong_) on_pong_();
                });

            w.set_option(ws::stream_base::decorator(
                [bearer](ws::request_type& req)
                {
                    if (!bearer.empty())
                        req.set(beast::http::field::authorization, "Bearer " + bearer);
                }));

            w.async_handshake(
                host,
                url_.target,
                [this](beast::error_code ec)
                {
                    if (ec) return fail(ec, "handshake");
                    if (on_open_) on_open_();
                    do_read();
                });
        });
}

void ws_connection::do_read()
{
    with_ws([this](auto& w)
        {
            bool is_text = w.got_text();

            w.async_read(
                buffer_,
                [this, is_text](beast::error_code ec, std::size_t n)
                {
                    if (ec)
                    {
                        if (ec != ws::error::closed)
                            if (on_error_) on_error_(ec.value(), ec.message());

                        if (on_close_) on_close_();
                        return;
                    }

                    std::string data(
                        static_cast<const char*>(buffer_.data().data()),
                        n);

                    buffer_.consume(n);

                    if (on_data_) on_data_(data.data(), (int)data.size(), is_text);
                    do_read();
                });
        });

}
void ws_connection::send_ping()
{
    asio::post(strand_, [this]()
        {
            if (!is_open()) return;

            with_ws([this](auto& w)
                {
                    w.async_ping({}, [this](beast::error_code ec)
                        {
                            if (ec) fail(ec, "ping");
                        });
                });
        });
}

void ws_connection::send_text(const std::string& text)
{
    asio::post(strand_, [this, text]()
        {
            if (!is_open()) return;

            with_ws([this, text](auto& w)
                {
                    w.text(true);
                    w.async_write(
                        asio::buffer(text),
                        [this](beast::error_code ec, std::size_t)
                        {
                            if (ec) fail(ec, "write");
                        });
                });
        });
}

void ws_connection::close()
{
    asio::post(strand_, [this]()
        {
            with_ws([](auto& w)
                {
                    beast::error_code ec;
                    w.close(ws::close_code::normal, ec);
                });
            plain_.reset();
            secure_.reset();
        });
}

template<class Fn>
void ws_connection::with_ws(Fn&& fn)
{
    if (plain_)
        return fn(*plain_);
    if (secure_)
        return fn(*secure_);
}

void ws_connection::fail(beast::error_code ec, const char* where)
{
    if (on_error_)
        on_error_((int)ec.value(), std::string(where) + ": " + ec.message());
}