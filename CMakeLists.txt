# open-ocharts-query CMakeLists.txt (cleaned for Linux-only build)

cmake_minimum_required(VERSION 3.10)
project(open-ocharts-query LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hide all by default, but explicitly export required classes
# add_compile_options(-Wall -fvisibility=hidden -fvisibility-inlines-hidden)

# Ensure RTTI and vtables are exported
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--export-dynamic")




# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/provider/include
    ${CMAKE_SOURCE_DIR}/provider/include/ocpn-import
    ${CMAKE_SOURCE_DIR}/provider/include/requestHandler
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/wxcurl/include
    ${CMAKE_SOURCE_DIR}/wxcurl/src
    ${wxWidgets_INCLUDE_DIRS}
)

# Define the sources explicitly
set(SRCS
  provider/src/ocpn-import/Viewport.cpp
  provider/src/ocpn-import/cutil.c
  provider/src/ocpn-import/ocpndc.cpp
  provider/src/ocpn-import/georef.cpp
  provider/src/ocpn-import/pluginmanager.cpp
  provider/src/ocpn-import/bbox.cpp
  provider/src/ChartInfo.cpp
  provider/src/ChartList.cpp
  provider/src/Renderer.cpp
  provider/src/Logger.cpp
  provider/src/RefCount.cpp
  provider/src/CacheHandler.cpp
  provider/src/ChartSetInfo.cpp
  provider/src/ChartManager.cpp
  provider/src/TokenHandler.cpp
  provider/src/MD5.cpp
  provider/src/CacheFiller.cpp
  provider/src/ChartSet.cpp
  provider/src/SystemHelper.cpp
  provider/src/StatusCollector.cpp
  provider/src/SettingsManager.cpp
  provider/src/MainQueue.cpp
  provider/src/ColorTable.cpp
  provider/src/S57AttributeDecoder.cpp
  provider/src/HTTPd/Worker.cpp
  provider/src/HTTPd/HTTPServer.cpp
  provider/src/TestHelper.cpp
  provider/src/main.cpp
)

add_executable(${PROJECT_NAME} ${SRCS})

set_target_properties(${PROJECT_NAME} PROPERTIES ENABLE_EXPORTS ON)
target_link_options(${PROJECT_NAME} PRIVATE -Wl,--export-dynamic)

# OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)

# wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS net xml html adv aui core base gl)
include(${wxWidgets_USE_FILE})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

# gettext (used by original)
find_package(Gettext REQUIRED)
target_link_libraries(${PROJECT_NAME} ${GETTEXT_LIBRARY})

# LibELF (optional, for plugin compatibility detection)
find_path(LIBELF_INCLUDE_DIR NAMES libelf.h gelf.h PATH_SUFFIXES libelf)
find_library(LIBELF_LIBRARY NAMES elf)
if (LIBELF_INCLUDE_DIR AND LIBELF_LIBRARY)
  target_include_directories(${PROJECT_NAME} PUBLIC ${LIBELF_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${LIBELF_LIBRARY})
endif()

# Add and link wxcurl
add_subdirectory(provider/libs/wxcurl)
# target_include_directories(${PROJECT_NAME} PRIVATE provider/libs/wxcurl/include)

target_link_libraries(${PROJECT_NAME}
    -Wl,--whole-archive
    ocpn::wxcurl
    -Wl,--no-whole-archive
    ${wxWidgets_LIBRARIES}
    ${OPENGL_LIBRARIES}
    dl
)


# Link against dl for Unix
if (UNIX)
  target_link_libraries(${PROJECT_NAME} dl)
endif()

