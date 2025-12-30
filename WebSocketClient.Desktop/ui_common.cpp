#include "ui_common.h"
#include <chrono>

static COLORREF g_status_color = RGB(0, 0, 0);

void UiSetStatusColor(COLORREF color)
{
    g_status_color = color;
}

COLORREF UiGetStatusColor()
{
    return g_status_color;
}

std::wstring NowTime()
{
    using namespace std::chrono;

    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = system_clock::to_time_t(now);

    tm local{};
    localtime_s(&local, &t);

    wchar_t buf[64];
    swprintf(buf, 64, L"%02d:%02d:%02d.%03d",
        local.tm_hour, local.tm_min, local.tm_sec, (int)ms.count());

    return buf;
}

std::wstring Utf8ToWide(const std::string& s)
{
    if (s.empty()) return L"";
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring out(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), out.data(), len);
    return out;
}

std::string WideToUtf8(const std::wstring& ws)
{
    if (ws.empty()) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), nullptr, 0, nullptr, nullptr);
    std::string out(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), out.data(), len, nullptr, nullptr);
    return out;
}

std::wstring GetWindowTextWString(HWND hWnd)
{
    int len = GetWindowTextLengthW(hWnd);
    std::wstring s(len, L'\0');
    GetWindowTextW(hWnd, s.data(), len + 1);
    return s;
}