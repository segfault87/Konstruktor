find_package(SharedMimeInfo REQUIRED)

add_definitions(${SQLITE_DEFINITIONS})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SQLITE_INCLUDE_DIR})

# Konstruktor

if(NO_KDE)
set(konstruktor_platform_SRCS
)
else(NO_KDE)
set(konstruktor_platform_SRCS
  kde/mainwindow.cpp
)
endif(NO_KDE)

set(konstruktor_SRCS
  ${konstruktor_platform_SRCS}
  application.cpp
  colordialog.cpp
  colormanager.cpp
  commandbase.cpp
  commandcolor.cpp
  commandinsert.cpp
  commandpaste.cpp
  commandremove.cpp
  commandtransform.cpp
  commandtransformlinear.cpp
  configdialog.cpp
  contentsmodel.cpp
  contentsview.cpp
  dbmanager.cpp
  document.cpp
  editor.cpp
  mainwindow.cpp
  newmodeldialog.cpp
  newsubmodeldialog.cpp
  objectlist.cpp
  partitems.cpp
  partsiconwidget.cpp
  partsmodel.cpp
  partswidget.cpp
  povrayexporter.cpp
  povrayrenderparameters.cpp
  povrayrenderwidget.cpp
  pixmapextension.cpp
  pixmaprenderer.cpp
  refobject.cpp
  renderwidget.cpp
  scanlinewidget.cpp
  selection.cpp
  submodelmodel.cpp
  submodelwidget.cpp
  undostackextension.cpp
  utils.cpp
  viewport.cpp
  visibilityextension.cpp
  main.cpp
)

kde4_add_kcfg_files(konstruktor_SRCS kde/config.kcfgc)
kde4_add_ui_files(konstruktor_SRCS
  colordialog.ui
  configdisplayform.ui
  configrenderform.ui
  newmodeldialog.ui
  newsubmodeldialog.ui
  partswidget.ui
)

kde4_add_executable(konstruktor ${konstruktor_SRCS})
target_link_libraries(konstruktor libldrawrenderer ${SQLITE_LIBRARIES} ${KDE4_KDECORE_LIBS}
${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY})

# Konstruktor DB Updater

set(konstruktor_db_updater_SRCS
  dbmanager.cpp
  dbupdater.cpp
  pixmaprenderer.cpp
  viewport.cpp
  main_dbupdater.cpp
)

kde4_add_kcfg_files(konstruktor_db_updater_SRCS kde/config.kcfgc)
kde4_add_executable(konstruktor_db_updater ${konstruktor_db_updater_SRCS})

set_target_properties(konstruktor_db_updater PROPERTIES COMPILE_FLAGS -DKONSTRUKTOR_DB_UPDATER)
target_link_libraries(konstruktor_db_updater libldrawrenderer ${SQLITE_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY})

# install
install(TARGETS konstruktor konstruktor_db_updater DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES konstruktor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES kde/konstruktorui.rc DESTINATION ${DATA_INSTALL_DIR}/konstruktor)
install(FILES ldraw-mime.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
kde4_install_icons(${ICON_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})

# subdirectories
add_subdirectory(icons)