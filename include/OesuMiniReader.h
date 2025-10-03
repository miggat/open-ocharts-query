// #pragma once
// #include <string>
// #include <vector>

// struct OesuCellInfo {
//     std::string oesuPath;   // ruta completa al .oesu
//     std::string metaDir;    // carpeta con metadatos (si encontrada)
//     std::string name;       // nombre de celda (stem del archivo, por defecto)
//     double west = 0, east = 0, south = 0, north = 0;
//     bool hasExtent = false;
// };

// class OesuMiniReader {
// public:
//     // Escanea un directorio recursivamente, detecta .oesu y busca metadatos
//     static bool LoadCellsFromDir(const std::string& dir, std::vector<OesuCellInfo>& out, std::string* err = nullptr);

//     // Dada la ruta al .oesu, intenta encontrar la carpeta “sidecar” de metadatos
//     static bool ProbeCell(const std::string& oesuPath, OesuCellInfo& out);

// private:
//     static bool findSidecarMeta(const std::string& oesuPath, std::string& metaDir);
//     static bool parseAnyTxtForExtent(const std::string& metaDir, OesuCellInfo& info);
//     static bool parseExtentLine(const std::string& line, OesuCellInfo& info, bool& touched);
// };
