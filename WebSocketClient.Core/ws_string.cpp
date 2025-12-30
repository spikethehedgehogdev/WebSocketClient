#include "ws_string.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace ws_string
{
#ifdef _WIN32

    std::string to_utf8(const wchar_t* wide)
    {
        if (!wide || !*wide)
            return {};

        int len = WideCharToMultiByte(
            CP_UTF8, 0,
            wide, -1,
            nullptr, 0,
            nullptr, nullptr);

        std::string out(len - 1, '\0');

        WideCharToMultiByte(
            CP_UTF8, 0,
            wide, -1,
            out.data(), len,
            nullptr, nullptr);

        return out;
    }

    std::string to_utf8(const std::string& native)
    {
        if (native.empty())
            return {};

        int wlen = MultiByteToWideChar(
            CP_ACP, 0,
            native.c_str(), -1,
            nullptr, 0);

        std::wstring wide(wlen - 1, L'\0');

        MultiByteToWideChar(
            CP_ACP, 0,
            native.c_str(), -1,
            wide.data(), wlen);

        return to_utf8(wide.c_str());
    }

#else

    std::string to_utf8(const std::string& native)
    {
        return native;
    }

#endif
}