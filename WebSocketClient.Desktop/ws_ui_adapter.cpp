#include "ui_common.h"
#include "ui_logger.h"
#include "ws_ui_adapter.h"
#include <windows.h>

static void cb_connected(void*, int connected)
{
    UiPostStatus(connected ? L"CONNECTED" : L"DISCONNECTED");
    UiPostLog(connected ? L"[ws] connected" : L"[ws] disconnected");
}

static void cb_timeout(void*)
{
    UiPostLog(L"[ws] pong timeout -> reconnect");
}

static void cb_reconnect(void*, int attempt, int delay_ms)
{
    wchar_t buf[256];
    wsprintfW(buf, L"[ws] reconnect attempt=%d delay=%dms", attempt, delay_ms);
    UiPostLog(buf);
}

static void cb_data(void*, const char* data, int size, int is_text)
{
    std::string s;
    if (data && size > 0)
        s.assign(data, data + size);

    std::wstring w = Utf8ToWide(s);
    UiPostLog((is_text ? L"[data:text] " : L"[data:bin] ") + w);
}

static void cb_error(void*, int code, const char* message)
{
    std::wstring wmsg = message ? Utf8ToWide(message) : L"";
    wchar_t buf[512];
    wsprintfW(buf, L"[error] code=%d msg=%s", code, wmsg.c_str());
    UiPostLog(buf);
}

static void cb_log(void*, const char* msg)
{
    if (msg)
        UiPostLog(Utf8ToWide(msg));
}

ws_client_callbacks MakeUiCallbacks()
{
    ws_client_callbacks c{};
    c.on_connected = cb_connected;
    c.on_pong_timeout = cb_timeout;
    c.on_reconnect = cb_reconnect;
    c.on_data = cb_data;
    c.on_error = cb_error;
    c.on_log = cb_log;
    return c;
}