cmake_minimum_required(VERSION 3.5)
project(lomiri-docviewer-app VERSION 3.0.0 LANGUAGES CXX)

set(MANIFEST_PATH "click/manifest.json.in" CACHE INTERNAL "Relative path to the manifest file")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-permissive -pedantic -Wall -Wextra -fPIC")

# Debugging purpose. Keep commented unless you need it.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")

include(FindPkgConfig)
include(GNUInstallDirs)

option(INSTALL_TESTS    "Install the tests on make install" on)
option(CLICK_MODE       "Installs to a contained location" on)
option(NO_CACHE         "Disable caching of click dependencies (CLICK_MODE only)" off)

set(DEFAULT_ROOT "/opt/click.ubuntu.com" CACHE PATH "Default click package root path")
set(LIBREOFFICE_INCLUDE_DIR "" CACHE PATH "Custom LibreOfficeKit include directory")

set(APP_NAME            lomiri-docviewer-app)
set(DESKTOP_FILE        "${PROJECT_NAME}.desktop")
set(ICON_FILE           docviewer-app.png)
set(SPLASH_FILE         docviewer-app-splash.svg)
set(AUTOPILOT_DIR       lomiri_docviewer_app)

# Find out the architecture for package building
# to determine the plugin's installation path
execute_process(
  COMMAND dpkg-architecture -qDEB_HOST_MULTIARCH
  OUTPUT_VARIABLE ARCH_TRIPLET
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND dpkg-architecture -qDEB_HOST_ARCH
  OUTPUT_VARIABLE CLICK_ARCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT CLICK_MODE)
    install(FILES lomiri-docviewer-app.1 DESTINATION "${MANDIR}/man1")
endif(NOT CLICK_MODE)

include(Click)

# Tests
enable_testing()

add_subdirectory(po)
add_subdirectory(click)
add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(tests)
