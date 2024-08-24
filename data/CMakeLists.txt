if(CLICK_MODE)
  set(ICON ${ICON_FILE})
  set(SPLASH ${SPLASH_FILE})
  set(DESKTOP_DIR ${DATA_DIR})
  set(EXEC "lomiri-docviewer-app-migrate.py ${APP_NAME}")

  install(FILES ${ICON_FILE} DESTINATION ${DATA_DIR})
  install(FILES ${SPLASH_FILE} DESTINATION ${DATA_DIR})
else(CLICK_MODE)
  set(ICON "${DATA_DIR}/${ICON_FILE}")
  set(SPLASH "${DATA_DIR}/${SPLASH_FILE}")
  set(DESKTOP_DIR ${CMAKE_INSTALL_DATADIR}/applications)
  set(EXEC "${APP_NAME}")

  install(FILES ${ICON_FILE} DESTINATION ${DATA_DIR})
  install(FILES ${SPLASH_FILE} DESTINATION ${DATA_DIR})
endif(CLICK_MODE)

set(TRYEXEC "${APP_NAME}")

configure_file(${DESKTOP_FILE}.in.in ${DESKTOP_FILE}.in @ONLY)

configure_file(${DESKTOP_FILE}.in.in ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in)
add_custom_target(${DESKTOP_FILE} ALL
    COMMENT "Merging translations into ${DESKTOP_FILE}"
    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE}
            --desktop --template=${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in
            -o ${DESKTOP_FILE}
            -d ${CMAKE_SOURCE_DIR}/po
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
  DESTINATION ${DESKTOP_DIR}
)
