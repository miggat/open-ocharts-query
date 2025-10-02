cmake_minimum_required(VERSION 3.16)
project(open-ocharts-query-clean CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# wxBase (sin GUI)
find_package(wxWidgets REQUIRED COMPONENTS base net xml)
include(${wxWidgets_USE_FILE})

# GDAL
find_package(GDAL REQUIRED)

# --- Target ---
add_executable(open-ocharts-query
    src/main.cpp
    src/HttpServer.cpp

    # O-charts core
    src/ocharts/Osenc.cpp
    src/ocharts/s57classregistrar.cpp
    src/ocharts/s57RegistrarMgr.cpp
    src/ocharts/mygeomOC.cpp
    src/ocharts/bbox.cpp
    src/ocharts/s57obj.cpp

    # shims y globals
    src/ocpn_plugin_shim.cpp
    src/globals.cpp
)

# Includes
target_include_directories(open-ocharts-query PRIVATE
    include
    include/ocharts
    ${GDAL_INCLUDE_DIRS}
)

# Link
# Preferir el target moderno si existe
if (TARGET GDAL::GDAL)
  target_link_libraries(open-ocharts-query PRIVATE GDAL::GDAL)
else()
  target_link_libraries(open-ocharts-query PRIVATE ${GDAL_LIBRARIES})
endif()

target_link_libraries(open-ocharts-query PRIVATE ${wxWidgets_LIBRARIES})
