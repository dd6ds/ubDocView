if(CLICK_MODE)
  configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json DESTINATION ${CMAKE_INSTALL_PREFIX})
  install(FILES docviewer.apparmor docviewer-content.json lomiri-docviewer-app.url-dispatcher DESTINATION ${CMAKE_INSTALL_PREFIX})
else(CLICK_MODE)
  install(FILES docviewer-content.json DESTINATION ${CMAKE_INSTALL_DATADIR}/lomiri-content-hub/peers RENAME lomiri-docviewer-app)
  install(FILES lomiri-docviewer-app.url-dispatcher DESTINATION ${CMAKE_INSTALL_DATADIR}/lomiri-url-dispatcher/urls)
endif(CLICK_MODE)


# Make the click files visible in Qt Creator
file(GLOB CLICK_FILES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  *.json *.json.in *.apparmor
)

add_custom_target(docviewer_ubports_CLICKFiles ALL SOURCES ${CLICK_FILES})
