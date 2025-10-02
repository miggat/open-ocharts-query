#include "HttpServer.h"
#include <iostream>

HttpServer::HttpServer(int port, const wxString& chartsDir, s57RegistrarMgr* registrar)
    : port(port), chartsDir(chartsDir), registrar(registrar)
{}

void HttpServer::start() {
    std::cout << "HTTP server listening on port " << port
              << " serving charts from " << chartsDir << "\n";
    // TODO: /list y /featureInfo usando registrar cuando toque
}
