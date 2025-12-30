#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include "ws_client.h"
#include "ui_common.h"
#include "ui_logger.h"
#include "ws_ui_adapter.h"

static const int IDC_URL_EDIT = 1001;
static const int IDC_TOKEN_EDIT = 1002;
static const int IDC_CONNECT_BTN = 1003;
static const int IDC_DISCONNECT_BTN = 1004;
static const int IDC_SEND_EDIT = 1005;
static const int IDC_SEND_BTN = 1006;
static const int IDC_LOG_EDIT = 1007;
static const int IDC_STATUS_STATIC = 1008;
static const int IDC_DEBUG_CHECK = 1009;

static HINSTANCE g_hInst = nullptr;
static HWND g_hMain = nullptr;
static HWND g_hUrl = nullptr;
static HWND g_hToken = nullptr;
static HWND g_hSend = nullptr;
static HWND g_hLog = nullptr;
static HWND g_hStatus = nullptr;

static ws_client_t* g_client = nullptr;

static void DoConnect()
{
    UiLogDebug(L"[ui] connect clicked");
    UiSetStatusColor(RGB(0, 0, 180));
    UiPostStatus(L"CONNECTING...");

    const std::string url = WideToUtf8(GetWindowTextWString(g_hUrl));
    const std::string token = WideToUtf8(GetWindowTextWString(g_hToken));

    ws_client_set_ping(g_client, 5000, 12000);
    auto cbs = MakeUiCallbacks();
    ws_client_connect(g_client, url.c_str(), token.c_str(), &cbs, nullptr);
}

static void DoDisconnect()
{
    UiLogDebug(L"[ui] disconnect clicked");
    ws_client_disconnect(g_client);
}

static void DoSend()
{
    UiLogDebug(L"[ui] send clicked");
    const std::string text = WideToUtf8(GetWindowTextWString(g_hSend));
    ws_client_send_text(g_client, text.c_str());
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        g_hMain = hWnd;
        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        CreateWindowW(L"STATIC", L"URL:", WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, hWnd, 0, g_hInst, 0);
        g_hUrl = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"wss://ws.ifelse.io",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 10, 30, 560, 24, hWnd, (HMENU)IDC_URL_EDIT, g_hInst, 0);

        CreateWindowW(L"STATIC", L"Token:", WS_CHILD | WS_VISIBLE, 10, 60, 80, 20, hWnd, 0, g_hInst, 0);
        g_hToken = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 10, 80, 560, 24, hWnd, (HMENU)IDC_TOKEN_EDIT, g_hInst, 0);

        CreateWindowW(L"BUTTON", L"Connect", WS_CHILD | WS_VISIBLE,
            580, 30, 120, 24, hWnd, (HMENU)IDC_CONNECT_BTN, g_hInst, 0);

        CreateWindowW(L"BUTTON", L"Disconnect", WS_CHILD | WS_VISIBLE,
            580, 80, 120, 24, hWnd, (HMENU)IDC_DISCONNECT_BTN, g_hInst, 0);

        CreateWindowW(L"STATIC", L"Status:", WS_CHILD | WS_VISIBLE,
            10, 112, 60, 20, hWnd, 0, g_hInst, 0);

        g_hStatus = CreateWindowW(L"STATIC", L"DISCONNECTED", WS_CHILD | WS_VISIBLE,
            75, 112, 300, 20, hWnd, (HMENU)IDC_STATUS_STATIC, g_hInst, 0);

        UiSetStatusColor(RGB(180, 0, 0));
        SetWindowTextW(g_hStatus, L"DISCONNECTED");
        InvalidateRect(g_hStatus, nullptr, TRUE);

        CreateWindowW(L"STATIC", L"Send:", WS_CHILD | WS_VISIBLE,
            10, 140, 80, 20, hWnd, 0, g_hInst, 0);

        g_hSend = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"hello",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 10, 160, 560, 24, hWnd, (HMENU)IDC_SEND_EDIT, g_hInst, 0);

        CreateWindowW(L"BUTTON", L"Send", WS_CHILD | WS_VISIBLE,
            580, 160, 120, 24, hWnd, (HMENU)IDC_SEND_BTN, g_hInst, 0);

        g_hLog = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
            10, 195, 690, 355, hWnd, (HMENU)IDC_LOG_EDIT, g_hInst, 0);

        CreateWindowW(
            L"BUTTON", L"Debug log",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            580, 112, 120, 24,
            hWnd, (HMENU)IDC_DEBUG_CHECK, g_hInst, 0
        );

        SendMessageW(g_hUrl, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(g_hToken, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(g_hSend, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(g_hLog, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(g_hStatus, WM_SETFONT, (WPARAM)hFont, TRUE);

        UiInit(hWnd, g_hLog, g_hStatus);
        g_client = ws_client_create();
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CONNECT_BTN:    DoConnect();    return 0;
        case IDC_DISCONNECT_BTN: DoDisconnect(); return 0;
        case IDC_SEND_BTN:       DoSend();       return 0;
        case IDC_DEBUG_CHECK:
        {
            bool enabled = SendMessageW((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED;
            UiSetDebugLog(enabled);
            return 0;
        }
        }
        break;
    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        HWND hCtrl = (HWND)lParam;

        if (hCtrl == g_hStatus)
        {
            SetTextColor(hdc, UiGetStatusColor());
            SetBkMode(hdc, TRANSPARENT);
            return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
        }
        break;
    }
    case WM_APP + 1:
    {
        auto* s = reinterpret_cast<std::wstring*>(lParam);
        UiAppendLog(*s);
        delete s;
        return 0;
    }
    case WM_APP + 2:
    {
        auto* s = reinterpret_cast<std::wstring*>(lParam);
        SetWindowTextW(g_hStatus, s->c_str());
        InvalidateRect(g_hStatus, nullptr, TRUE);
        delete s;
        return 0;
    }
    case WM_DESTROY:
        ws_client_destroy(g_client);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    g_hInst = hInstance;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"WebSocketClientWnd";

    RegisterClassExW(&wc);

    HWND hWnd = CreateWindowW(
        wc.lpszClassName,
        L"WebSocketClient",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 730, 610,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}