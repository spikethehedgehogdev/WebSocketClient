#pragma once
#include "ws_client.h"

class event_dispatcher
{
public:
    void set(const ws_client_callbacks& cbs, void* user);

    void connected(bool);
    void timeout();
    void reconnect(int attempt, int delay);
    void data(const char* d, int s, bool text);
    void error(int code, const char* msg);
    void log(const char* msg);

private:
    ws_client_callbacks cbs_{};
    void* user_ = nullptr;
};