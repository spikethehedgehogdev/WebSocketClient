#pragma once

#include <string>

namespace ws_string
{
    std::string to_utf8(const std::string& native);

#ifdef _WIN32
    std::string to_utf8(const wchar_t* wide);
#endif
}