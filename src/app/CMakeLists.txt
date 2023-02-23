file(GLOB_RECURSE QML_SRCS *.qml *.js)

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Qml)
find_package(Qt5Quick)

add_definitions(
  -DGETTEXT_PACKAGE=\"${PROJECT_NAME}\"
  -DGETTEXT_LOCALEDIR=\"${CMAKE_INSTALL_LOCALEDIR}\"
)

string(REGEX REPLACE "^/*(.+)/*$" "\\1" CLICK_REL_PATH "${DEFAULT_ROOT}")

configure_file(
    config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(docviewer_SRCS
    main.cpp
    renderengine.cpp
    rendertask.cpp
    ${QML_SRCS}
)

add_executable(lomiri-docviewer-app ${docviewer_SRCS})
set_property(TARGET lomiri-docviewer-app PROPERTY ENABLE_EXPORTS True)

qt5_use_modules(lomiri-docviewer-app Gui Qml Quick)
target_link_libraries(lomiri-docviewer-app stdc++)

if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(docviewer-qmlfiles ALL
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/qml ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${QMLFILES}
)
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

install(DIRECTORY qml DESTINATION ${DATA_DIR})

if(CLICK_MODE)
  install(TARGETS lomiri-docviewer-app DESTINATION ${BIN_DIR})
else()
  install(TARGETS lomiri-docviewer-app RUNTIME DESTINATION bin)
endif()

install(PROGRAMS lomiri-docviewer-app-migrate.py DESTINATION ${DATA_DIR})
