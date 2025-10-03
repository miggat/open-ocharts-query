#include "HttpServer.h"

#include <wx/init.h>     // wxInitializer
#include <wx/string.h>   // wxString
#include <iostream>
#include <cstdlib>
#include "ocharts/Osenc.h"



int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <charts_dir> [port]\n";
        return 1;
    }

    wxInitializer init;
    if ( !init.IsOk() ) {
        std::cerr << "Failed to initialize wxBase\n";
        return 1;
    }

    wxString chartsDir = wxString::FromUTF8(argv[1]);
    int port = (argc > 2) ? std::atoi(argv[2]) : 10689;

    HttpServer server(port, chartsDir);
    server.start();

    // while(true){
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // }

    return 0;
}
