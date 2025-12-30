#pragma once
#include <string>
#include <windows.h>

void UiInit(HWND hMain, HWND hLog, HWND hStatus);

void UiAppendLog(const std::wstring& line);
void UiPostLog(const std::wstring& line);
void UiPostStatus(const std::wstring& status);