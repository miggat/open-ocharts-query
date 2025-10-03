// #include "OesuMiniReader.h"
// #include <filesystem>
// #include <fstream>
// #include <iostream>
// #include <sstream>
// #include <algorithm>
// #include <cctype>

// namespace fs = std::filesystem;

// static std::string toLower(std::string s){
//     std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
//     return s;
// }

// bool OesuMiniReader::LoadCellsFromDir(const std::string& dir, std::vector<OesuCellInfo>& out, std::string* err) {
//     out.clear();
//     if (!fs::exists(dir) || !fs::is_directory(dir)) {
//         if (err) *err = "OesuMiniReader: dir not found: " + dir;
//         return false;
//     }
//     int found = 0;
//     for (auto& de : fs::recursive_directory_iterator(dir)) {
//         if (!de.is_regular_file()) continue;
//         const auto& p = de.path();
//         if (p.has_extension() && toLower(p.extension().string()) == ".oesu") {
//             OesuCellInfo info;
//             if (ProbeCell(p.string(), info)) {
//                 out.push_back(std::move(info));
//                 ++found;
//             } else {
//                 // no meta encontrado; añadimos igualmente con name básico
//                 OesuCellInfo fallback;
//                 fallback.oesuPath = p.string();
//                 fallback.name = p.stem().string();
//                 out.push_back(std::move(fallback));
//                 ++found;
//             }
//         }
//     }
//     if (!found) {
//         if (err) *err = "OesuMiniReader: no .oesu found in: " + dir;
//         return false;
//     }
//     return true;
// }

// bool OesuMiniReader::ProbeCell(const std::string& oesuPath, OesuCellInfo& out) {
//     out = {};
//     out.oesuPath = oesuPath;
//     out.name = fs::path(oesuPath).stem().string();

//     std::string meta;
//     if (!findSidecarMeta(oesuPath, meta)) {
//         return false;
//     }
//     out.metaDir = meta;

//     // escanea .txt en metaDir para extraer W/E/S/N
//     if (!parseAnyTxtForExtent(meta, out)) {
//         // meta existe pero sin extent
//         return true;
//     }
//     return true;
// }

// bool OesuMiniReader::findSidecarMeta(const std::string& oesuPath, std::string& metaDir) {
//     // candidatos frecuentes:
//     //  - mismo stem como carpeta a la par del .oesu
//     //  - carpeta con sufijos/prefijos comunes
//     fs::path p(oesuPath);
//     auto parent = p.parent_path();
//     const std::string stem = p.stem().string();

//     const std::vector<std::string> candidates = {
//         stem,                       // /dir/<stem>/
//         stem + "_meta",             // /dir/<stem>_meta/
//         stem + "-meta",             // /dir/<stem>-meta/
//         stem + ".meta",             // /dir/<stem>.meta/
//         stem + "_OESU",             // /dir/<stem>_OESU/
//         stem + ".OESU"              // /dir/<stem>.OESU/
//     };

//     for (const auto& c : candidates) {
//         fs::path d = parent / c;
//         if (fs::exists(d) && fs::is_directory(d)) {
//             metaDir = d.string();
//             return true;
//         }
//     }
//     return false;
// }

// bool OesuMiniReader::parseAnyTxtForExtent(const std::string& metaDir, OesuCellInfo& info) {
//     bool got = false;
//     for (auto& de : fs::directory_iterator(metaDir)) {
//         if (!de.is_regular_file()) continue;
//         const auto& p = de.path();
//         if (!p.has_extension()) continue;
//         const auto ext = toLower(p.extension().string());
//         if (ext != ".txt") continue;

//         std::ifstream ifs(p);
//         if (!ifs) continue;
//         std::string line;
//         while (std::getline(ifs, line)) {
//             bool touched = false;
//             if (parseExtentLine(line, info, touched)) {
//                 got = true || got;
//             }
//         }
//     }
//     info.hasExtent = got && (info.west < info.east) && (info.south < info.north);
//     return info.hasExtent;
// }

// static bool parseKV(const std::string& line, std::string& k, std::string& v) {
//     auto pos = line.find_first_of("=:");
//     if (pos == std::string::npos) return false;
//     k = toLower(line.substr(0, pos));
//     v = line.substr(pos+1);
//     // trim
//     auto ltrim = [](std::string& s){
//         s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c){ return !std::isspace(c); }));
//     };
//     auto rtrim = [](std::string& s){
//         s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c){ return !std::isspace(c); }).base(), s.end());
//     };
//     ltrim(k); rtrim(k); ltrim(v); rtrim(v);
//     return true;
// }

// bool OesuMiniReader::parseExtentLine(const std::string& line, OesuCellInfo& info, bool& touched) {
//     touched = false;
//     std::string k, v;
//     if (!parseKV(line, k, v)) return false;

//     auto toD = [](const std::string& s)->double{
//         try { return std::stod(s); } catch(...) { return 0.0; }
//     };

//     // claves típicas
//     if (k == "w" || k == "west" || k == "minx" || k == "xmin") {
//         info.west = toD(v); touched = true; return true;
//     }
//     if (k == "e" || k == "east" || k == "maxx" || k == "xmax") {
//         info.east = toD(v); touched = true; return true;
//     }
//     if (k == "s" || k == "south" || k == "miny" || k == "ymin") {
//         info.south = toD(v); touched = true; return true;
//     }
//     if (k == "n" || k == "north" || k == "maxy" || k == "ymax") {
//         info.north = toD(v); touched = true; return true;
//     }

//     // a veces viene como "bbox=minx,miny,maxx,maxy"
//     if (k == "bbox" || k == "bounds" || k == "extent") {
//         std::istringstream iss(v);
//         std::string a,b,c,d;
//         if (std::getline(iss, a, ',') &&
//             std::getline(iss, b, ',') &&
//             std::getline(iss, c, ',') &&
//             std::getline(iss, d, ',')) {
//             info.west  = toD(a);
//             info.south = toD(b);
//             info.east  = toD(c);
//             info.north = toD(d);
//             touched = true; return true;
//         }
//     }

//     // nombre de celda (si aparece)
//     if (k == "cellname" || k == "name" || k == "title") {
//         if (!v.empty()) info.name = v;
//         return true;
//     }

//     return false;
// }
