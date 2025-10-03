#include "HttpServer.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <charconv>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static optional<double> parseDouble(const string &s)
{
    double v{};
    auto *begin = s.data();
    auto *end = s.data() + s.size();
    auto res = std::from_chars(begin, end, v);
    if (res.ec == errc{} && res.ptr == end)
        return v;
    // fallback
    try
    {
        return stod(s);
    }
    catch (...)
    {
        return nullopt;
    }
}

HttpServer::HttpServer(int port, const wxString &chartsDir)
    : port(port), chartsDir(chartsDir)
{

    std::cout << "[FeatureInfoEngine] ready for dir: " << std::string(chartsDir.mb_str()) << "\n";
}

void HttpServer::start()
{
    std::cout << "HTTP server listening on port " << port
              << " serving charts from " << chartsDir << "\n";

    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "socket() failed: " << strerror(errno) << "\n";
        return;
    }

    int opt = 1;
    ::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (::bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "bind() failed: " << strerror(errno) << "\n";
        ::close(server_fd);
        return;
    }

    if (::listen(server_fd, 16) < 0)
    {
        std::cerr << "listen() failed: " << strerror(errno) << "\n";
        ::close(server_fd);
        return;
    }

    while (true)
    {
        int client_fd = ::accept(server_fd, nullptr, nullptr);
        if (client_fd < 0)
        {
            std::cerr << "accept() failed: " << strerror(errno) << "\n";
            continue;
        }
        handle_client(client_fd);
        ::close(client_fd);
    }
}

static map<string, string> parseQuery(const string &qs)
{
    map<string, string> out;
    size_t i = 0;
    while (i < qs.size())
    {
        size_t eq = qs.find('=', i);
        size_t amp = qs.find('&', i);
        if (eq == string::npos)
            break;
        size_t key_end = (amp == string::npos) ? qs.size() : min(eq, amp);
        string key = qs.substr(i, key_end - i);
        string val;
        if (eq < qs.size())
        {
            size_t val_begin = eq + 1;
            size_t val_end = (amp == string::npos) ? qs.size() : amp;
            if (val_begin <= val_end)
                val = qs.substr(val_begin, val_end - val_begin);
        }
        out[key] = val;
        if (amp == string::npos)
            break;
        i = amp + 1;
    }
    return out;
}

void HttpServer::handle_client(int client_fd)
{
    char buf[8192];
    ssize_t n = ::recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0)
        return;
    buf[n] = '\0';

    // Primera línea: "GET /path?query HTTP/1.1"
    std::istringstream iss{std::string(buf)};
    string method, target, httpv;
    iss >> method >> target >> httpv;

    // Simple router
    string path = target;
    string query;
    if (auto pos = target.find('?'); pos != string::npos)
    {
        path = target.substr(0, pos);
        query = target.substr(pos + 1);
    }

    auto send = [&](int code, const string &body)
    {
        std::ostringstream hdr;
        hdr << "HTTP/1.1 " << code << " OK\r\n"
            << "Content-Type: application/json\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n\r\n";
        const auto &hs = hdr.str();
        ::send(client_fd, hs.data(), hs.size(), 0);
        ::send(client_fd, body.data(), body.size(), 0);
    };

    if (method != "GET")
    {
        send(405, R"({"status":"error","message":"method not allowed"})");
        return;
    }

    if (path.rfind("/charts/", 0) == 0 && path.size() > strlen("/charts/"))
    {
        // endpoint esperado: /charts/<set>/featureInfo
        // extrae trailing path
        string rest = path.substr(strlen("/charts/"));
        // split en 2 partes: <set>/<endpoint>
        auto slash = rest.find('/');
        if (slash == string::npos)
        {
            send(404, R"({"status":"error","message":"not found"})");
            return;
        }
        string setName = rest.substr(0, slash);
        string endpoint = rest.substr(slash); // incluye '/'

        if (endpoint == "/featureInfo")
        {
            auto q = parseQuery(query);

            auto latOpt = parseDouble(q["lat"]);
            auto lonOpt = parseDouble(q["lon"]);
            auto tolOpt = parseDouble(q["tolerance"]);
            double tol = tolOpt.value_or(0.01);

            if (!latOpt || !lonOpt)
            {
                send(400, R"({"status":"error","message":"lat/lon required"})");
                return;
            }

            double lat = *latOpt;
            double lon = *lonOpt;

            // 🔹 TODO: aquí irá la llamada a Osenc/eSENCChart
            // De momento, devolvemos un dummy con lo que recibimos

            std::ostringstream body;
            body << R"({"status":"ok","count":1,"features":[)"
                 << R"({"layer":"OESU","class":"dummy","name":"placeholder")"
                 << R"(,"fid":1,"lon":)" << lon
                 << R"(,"lat":)" << lat
                 << R"(,"dist_deg":)" << tol << "}]}";

            std::cout << "[featureInfo] set=" << setName
                      << " lat=" << lat << " lon=" << lon
                      << " tol=" << tol << " -> dummy response\n";

            send(200, body.str());
            return;
        }
    }

    // fallback
    send(404, R"({"status":"error","message":"not found"})");
}
