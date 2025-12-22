#pragma once
#include <string>

struct ws_url
{
    bool secure;
    std::string host;
    std::string port;
    std::string target;
};

bool parse_ws_url(const std::string& url, ws_url& out, std::string& err);
std::string make_host_header(const ws_url& u);