#pragma once
#include "ws_client.h"

void UiSetDebugLog(bool enable);
void UiLogDebug(const wchar_t* msg);
void UiLogInfo(const wchar_t* msg);

ws_client_callbacks MakeUiCallbacks();