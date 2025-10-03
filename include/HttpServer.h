#pragma once
#include <wx/string.h>
#include <memory>
#include <string>

class HttpServer {
public:
    HttpServer(int port, const wxString& chartsDir);
    void start();

private:
    void handle_client(int client_fd);

    int port;
    wxString chartsDir;

};
