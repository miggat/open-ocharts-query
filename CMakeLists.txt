cmake_minimum_required(VERSION 3.16)
project(open-ocharts-query-clean CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# wxBase (sin GUI)
find_package(wxWidgets REQUIRED COMPONENTS base net xml)
include(${wxWidgets_USE_FILE})

# --- GDAL/S57 internos ---
file(GLOB GDAL_SOURCES
    libs/gdal/src/*.cpp
)

# --- iso 8211 internos ---
file(GLOB ISO8211_SOURCES
    libs/iso8211/src/*.cpp
)


# --- Target ---
add_executable(open-ocharts-query
    src/main.cpp
    src/ocpn_plugin_shim.cpp
    src/HttpServer.cpp
    src/OesuMiniReader.cpp
    src/ocharts/Osenc.cpp
    src/ocharts/eSENCChart.cpp
    src/ocharts/bbox.cpp
    src/globals.cpp

    ${ISO8211_SOURCES}
    ${GDAL_SOURCES}
)

# Includes
target_include_directories(open-ocharts-query PRIVATE
    libs/gdal/src   # para ogr_s57.h, gdal_priv.h, etc.
    include
    include/ocharts
)

# Link
target_link_libraries(open-ocharts-query PRIVATE ${wxWidgets_LIBRARIES})
