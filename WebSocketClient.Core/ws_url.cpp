#include "ws_url.h"

bool parse_ws_url(const std::string& url, ws_url& out, std::string& err)
{
    err.clear();
    auto pos = url.find("://");
    if (pos == std::string::npos) { err = "missing scheme"; return false; }

    auto scheme = url.substr(0, pos);
    auto rest = url.substr(pos + 3);

    out.secure = (scheme == "wss");
    if (!out.secure && scheme != "ws") { err = "bad scheme"; return false; }

    auto slash = rest.find('/');
    auto authority = (slash == std::string::npos) ? rest : rest.substr(0, slash);
    out.target = (slash == std::string::npos) ? "/" : rest.substr(slash);

    auto colon = authority.find(':');
    out.host = (colon == std::string::npos) ? authority : authority.substr(0, colon);
    out.port = (colon == std::string::npos)
        ? (out.secure ? "443" : "80")
        : authority.substr(colon + 1);

    return !out.host.empty();
}

std::string make_host_header(const ws_url& u)
{
    if ((u.secure && u.port == "443") || (!u.secure && u.port == "80"))
        return u.host;
    return u.host + ":" + u.port;
}