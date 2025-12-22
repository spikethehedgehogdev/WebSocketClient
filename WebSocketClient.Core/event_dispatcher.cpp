#include "event_dispatcher.h"

void event_dispatcher::set(const ws_client_callbacks& cbs, void* user)
{
    cbs_ = cbs;
    user_ = user;
}

void event_dispatcher::connected(bool v)
{
    if (cbs_.on_connected) cbs_.on_connected(user_, v ? 1 : 0);
}

void event_dispatcher::timeout()
{
    if (cbs_.on_pong_timeout) cbs_.on_pong_timeout(user_);
}

void event_dispatcher::reconnect(int a, int d)
{
    if (cbs_.on_reconnect) cbs_.on_reconnect(user_, a, d);
}

void event_dispatcher::data(const char* d, int s, bool t)
{
    if (cbs_.on_data) cbs_.on_data(user_, d, s, t ? 1 : 0);
}

void event_dispatcher::error(int c, const char* m)
{
    if (cbs_.on_error) cbs_.on_error(user_, c, m);
}

void event_dispatcher::log(const char* msg)
{
    if (cbs_.on_log && msg)
        cbs_.on_log(user_, msg);
}