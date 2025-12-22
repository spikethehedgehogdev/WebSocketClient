#include "ui_common.h"
#include "ui_logger.h"

static HWND g_hMain = nullptr;
static HWND g_hLog = nullptr;
static HWND g_hStatus = nullptr;

static const UINT WM_APP_LOG = WM_APP + 1;
static const UINT WM_APP_SETSTATUS = WM_APP + 2;

void UiInit(HWND hMain, HWND hLog, HWND hStatus)
{
    g_hMain = hMain;
    g_hLog = hLog;
    g_hStatus = hStatus;
}

void UiAppendLog(const std::wstring& line)
{
    if (!g_hLog) return;

    std::wstring msg = L"[" + NowTime() + L"] " + line + L"\r\n";

    int len = GetWindowTextLengthW(g_hLog);
    SendMessageW(g_hLog, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessageW(g_hLog, EM_REPLACESEL, FALSE, (LPARAM)msg.c_str());
}

void UiPostLog(const std::wstring& line)
{
    auto* s = new std::wstring(line);
    PostMessageW(g_hMain, WM_APP_LOG, 0, (LPARAM)s);
}

void UiPostStatus(const std::wstring& status)
{
    auto* s = new std::wstring(status);
    PostMessageW(g_hMain, WM_APP_SETSTATUS, 0, (LPARAM)s);
}