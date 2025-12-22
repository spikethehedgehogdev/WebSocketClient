#pragma once
#include <string>
#include <windows.h>

std::wstring NowTime();
std::wstring Utf8ToWide(const std::string&);
std::string  WideToUtf8(const std::wstring&);
std::wstring GetWindowTextWString(HWND);
