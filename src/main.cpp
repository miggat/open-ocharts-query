#include "HttpServer.h"
#include "ocharts/s57RegistrarMgr.h"
#include "ocharts/S57ClassRegistrar.h"

#include <wx/init.h> // wxInitializer
#include <wx/string.h>
#include <wx/filename.h>
#include <iostream>

// Definido en src/globals.cpp (debes tenerlo):
//   S57ClassRegistrar* pi_poRegistrar = nullptr;
extern S57ClassRegistrar *pi_poRegistrar;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <charts_dir> [port]\n";
        return 1;
    }

    // wxBase init (sin GUI)
    wxInitializer init;
    if (!init.IsOk())
    {
        std::cerr << "Failed to initialize wxBase.\n";
        return 1;
    }

    const wxString chartsDir = wxString::FromUTF8(argv[1]);
    const int port = (argc > 2) ? atoi(argv[2]) : 10689;

    // 1) Registrar S-57 (obj/attrs) y conectar el global requerido por s57RegistrarMgr
    S57ClassRegistrar registrar;
    pi_poRegistrar = &registrar;

    // 2) Inicializa el manager con la carpeta de CSVs S-57
    //    (ajusta la ruta según tu layout: p.ej. "<repo>/data/s57data")
    const wxString csvDir = wxString::FromUTF8("data/s57data");
    s57RegistrarMgr regMgr(csvDir, /*flog*/ nullptr);
    // (regMgr se queda en scope para toda la vida del proceso; ya ha cargado en 'registrar')

    // 3) Servidor HTTP (pásale el registrar para feature/class lookups)
    HttpServer server(port, chartsDir, &regMgr);
    server.start();

    return 0;
}
