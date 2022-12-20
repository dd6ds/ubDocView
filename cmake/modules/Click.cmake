if(CLICK_MODE)
  set(QT_IMPORTS_DIR "/lib/${ARCH_TRIPLET}")
  set(CMAKE_INSTALL_PREFIX /)
  set(DATA_DIR /)

  # Path for lomiri-docviewer-app executable
  set(BIN_DIR ${DATA_DIR}lib/${ARCH_TRIPLET}/bin)
else(CLICK_MODE)
  execute_process(
    COMMAND qmake -query QT_INSTALL_QML
    OUTPUT_VARIABLE QT_IMPORTS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if (QT_IMPORTS_DIR STREQUAL "")
     set(QT_IMPORTS_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/qt5/qml")
     message(STATUS "QT_IMPORTS_DIR is empty using default one: ${QT_IMPORTS_DIR}")
  else()
     message(STATUS "QT_IMPORTS_DIR set to ${QT_IMPORTS_DIR}")
  endif()

  set(DATA_DIR ${CMAKE_INSTALL_DATADIR}/${APP_NAME})
endif(CLICK_MODE)
