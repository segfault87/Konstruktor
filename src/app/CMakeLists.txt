if(UNIX AND NOT APPLE)
  find_package(SharedMimeInfo REQUIRED)
endif(UNIX AND NOT APPLE)

if(WIN32)
set(GUI_TYPE WIN32)
endif(WIN32)

add_definitions(${SQLITE_DEFINITIONS})
add_definitions(${QT_DEFINITIONS})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SQLITE_INCLUDE_DIR} ${QT_INCLUDES})

# Konstruktor

set(konstruktor_HEADERS
  aboutdialog.h
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
  dbupdaterdialog.h
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
  aboutdialog.cpp
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
  dbupdaterdialog.cpp
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
  aboutdialog.ui
  colordialog.ui
  configdialog.ui
  configdisplayform.ui
  configldrawform.ui
  configrenderform.ui
  newmodeldialog.ui
  newsubmodeldialog.ui
  partswidget.ui
)

qt4_add_resources(konstruktor_RCC_SRCS konstruktor.qrc)
qt4_wrap_cpp(konstruktor_HEADERS_MOC ${konstruktor_HEADERS})

add_executable(konstruktor ${GUI_TYPE} ${konstruktor_SRCS} ${konstruktor_UIS} ${konstruktor_HEADERS_MOC} ${konstruktor_RCC_SRCS})
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
install(TARGETS konstruktor konstruktor_db_updater DESTINATION bin)

if(UNIX AND NOT APPLE)
  install(FILES konstruktor.desktop DESTINATION share/applications)
  install(FILES ldraw-mime.xml DESTINATION share/mime/packages)
  update_xdg_mimetypes(share/mime/packages)
endif(UNIX AND NOT APPLE)

