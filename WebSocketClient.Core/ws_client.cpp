#include "ws_client.h"
#include "ws_client_impl.h"

#include <thread>
#include <memory>

struct ws_client_t
{
    boost::asio::io_context ioc;
    boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type> work_guard;

    std::thread thread;
    std::unique_ptr<ws_client_impl> impl;

    ws_client_t()
        : work_guard(boost::asio::make_work_guard(ioc))
    {
        impl = std::make_unique<ws_client_impl>(ioc);
        thread = std::thread([this]() { ioc.run(); });
    }

    ~ws_client_t()
    {
        work_guard.reset();
        ioc.stop();
        if (thread.joinable()) thread.join();
    }
};


extern "C" WSCLIENT_API ws_client_t* ws_client_create()
{
    try { return new ws_client_t(); }
    catch (...) { return nullptr; }
}

extern "C" WSCLIENT_API void ws_client_destroy(ws_client_t* c)
{
    delete c;
}

extern "C" WSCLIENT_API ws_result ws_client_connect(
    ws_client_t* c,
    const char* url,
    const char* token,
    const ws_client_callbacks* cb,
    void* user)
{
    if (!c || !url || !cb) return WS_INVALID_ARGUMENT;

    c->impl->set_callbacks(*cb, user);
    c->impl->connect(url, token ? token : "");
    return WS_OK;
}

extern "C" WSCLIENT_API void ws_client_disconnect(ws_client_t* c)
{
    if (c) c->impl->disconnect();
}

extern "C" WSCLIENT_API int ws_client_send_text(ws_client_t* c, const char* t)
{
    if (!c || !t) return 0;
    c->impl->send_text(t);
    return 1;
}

extern "C" WSCLIENT_API void ws_client_set_ping(
    ws_client_t* c, int ping_ms, int pong_timeout_ms)
{
    if (c) c->impl->set_ping(ping_ms, pong_timeout_ms);
}