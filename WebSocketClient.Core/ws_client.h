#pragma once
#include <cstdint>

#ifdef WEBSOCKETCLIENTCORE_EXPORTS
#define WSCLIENT_API __declspec(dllexport)
#else
#define WSCLIENT_API __declspec(dllimport)
#endif

extern "C" {

    struct ws_client_t;

    typedef void (*ws_on_connected_cb)(void*, int);
    typedef void (*ws_on_timeout_cb)(void*);
    typedef void (*ws_on_reconnect_cb)(void*, int, int);
    typedef void (*ws_on_data_cb)(void*, const char*, int, int);
    typedef void (*ws_on_error_cb)(void*, int, const char*);
    typedef void (*ws_on_log_cb)(void*, const char*);

    struct ws_client_callbacks
    {
        ws_on_connected_cb on_connected;
        ws_on_timeout_cb   on_pong_timeout;
        ws_on_reconnect_cb on_reconnect;
        ws_on_data_cb      on_data;
        ws_on_error_cb     on_error;
        ws_on_log_cb       on_log;
    };

    enum ws_result
    {
        WS_OK = 0,
        WS_INVALID_ARGUMENT = -1,
        WS_ALREADY_CONNECTED = -2
    };

    WSCLIENT_API ws_client_t* ws_client_create();

    WSCLIENT_API void ws_client_destroy(ws_client_t*);

    WSCLIENT_API ws_result ws_client_connect(
        ws_client_t*,
        const char* url,
        const char* bearer,
        const ws_client_callbacks*,
        void* user);

    WSCLIENT_API void ws_client_disconnect(ws_client_t*);
    WSCLIENT_API int  ws_client_send_text(ws_client_t*, const char*);
    WSCLIENT_API void ws_client_set_ping(ws_client_t*, int ping_ms, int pong_timeout_ms);

}