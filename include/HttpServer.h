#pragma once
#include <wx/string.h>

// fwd-decl para no arrastrar headers
class s57RegistrarMgr;

class HttpServer {
public:
    HttpServer(int port, const wxString& chartsDir, s57RegistrarMgr* registrar);
    void start();
private:
    int port;
    wxString chartsDir;
    s57RegistrarMgr* registrar;
};
