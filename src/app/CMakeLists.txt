find_package(SharedMimeInfo REQUIRED)

add_definitions(${SQLITE_DEFINITIONS})
add_definitions(${QT_DEFINITIONS})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SQLITE_INCLUDE_DIR})

# Konstruktor

set(konstruktor_HEADERS
  actionmanager.h
  application.h
  colordialog.h
  colormanager.h
  commandbase.h
  commandcolor.h
  commandinsert.h
  commandpaste.h
  commandremove.h
  commandtransform.h
  commandtransformlinear.h
  common.h
  configdialog.h
  config.h
  contentsmodel.h
  contentsview.h
  dbmanager.h
  document.h
  editor.h
  mainwindow.h
  menumanager.h
  newmodeldialog.h
  newsubmodeldialog.h
  objectlist.h
  partitems.h
  partsiconwidget.h
  partsmodel.h
  partswidget.h
  pivotextension.h
  pixmapextension.h
  pixmaprenderer.h
  povrayexporter.h
  povrayrenderparameters.h
  povrayrenderwidget.h
  refobject.h
  renderwidget.h
  scanlinewidget.h
  selection.h
  submodelmodel.h
  submodelwidget.h
  undostackextension.h
  utils.h
  viewport.h
  visibilityextension.h
)
  
set(konstruktor_SRCS
  actionmanager.cpp
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
  config.cpp
  configdialog.cpp
  contentsmodel.cpp
  contentsview.cpp
  dbmanager.cpp
  document.cpp
  editor.cpp
  mainwindow.cpp
  menumanager.cpp
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
  pivotextension.cpp
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

qt4_wrap_ui(konstruktor_UIS
  colordialog.ui
  configdisplayform.ui
  configrenderform.ui
  newmodeldialog.ui
  newsubmodeldialog.ui
  partswidget.ui
)

qt4_add_resources(konstruktor_RCC_SRCS konstruktor.qrc)
qt4_wrap_cpp(konstruktor_HEADERS_MOC ${konstruktor_HEADERS})

add_executable(konstruktor ${konstruktor_SRCS} ${konstruktor_UIS} ${konstruktor_HEADERS_MOC} ${konstruktor_RCC_SRCS})
target_link_libraries(konstruktor libldr libldrawrenderer ${SQLITE_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

# Konstruktor DB Updater

set(konstruktor_db_updater_HEADERS
  dbmanager.h
  dbupdater.h
  pixmaprenderer.h
  viewport.h
)

set(konstruktor_db_updater_SRCS
  config.cpp
  dbmanager.cpp
  dbupdater.cpp
  pixmaprenderer.cpp
  viewport.cpp
  main_dbupdater.cpp
)

qt4_wrap_cpp(konstruktor_db_updater_HEADERS_MOC ${konstruktor_db_updater_HEADERS})

add_executable(konstruktor_db_updater ${konstruktor_db_updater_SRCS} ${konstruktor_db_updater_HEADERS_MOC})
set_target_properties(konstruktor_db_updater PROPERTIES COMPILE_FLAGS -DKONSTRUKTOR_DB_UPDATER)
target_link_libraries(konstruktor_db_updater libldr libldrawrenderer ${SQLITE_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY})

# install
install(TARGETS konstruktor konstruktor_db_updater DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES konstruktor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ldraw-mime.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})

