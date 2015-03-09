if(UNIX AND NOT APPLE)
  find_package(SharedMimeInfo REQUIRED)
endif(UNIX AND NOT APPLE)

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
  dbupdater.h
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
  dbupdater.cpp
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

set(konstruktor_RESOURCES
  konstruktor.icns
)

qt5_wrap_ui(konstruktor_UIS
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

qt5_add_resources(konstruktor_RCC_SRCS konstruktor.qrc)
qt5_wrap_cpp(konstruktor_HEADERS_MOC ${konstruktor_HEADERS})

if(APPLE)
set_source_files_properties(konstruktor.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set(PROGNAME Konstruktor)
set(MACOSX_BUNDLE_ICON_FILE konstruktor.icns)
set(MACOSX_BUNDLE_BUNDLE_NAME "Konstruktor")
set(MACOSX_BUNDLE_INFO_STRING "LDraw Model Designer")
else(APPLE)
set(PROGNAME konstruktor)
endif(APPLE)

add_executable(${PROGNAME} MACOSX_BUNDLE WIN32 ${konstruktor_SRCS} ${konstruktor_HEADERS} ${konstruktor_UIS} ${konstruktor_HEADERS_MOC} ${konstruktor_RCC_SRCS} ${konstruktor_RESOURCES})
target_link_libraries(${PROGNAME} libldr libldrawrenderer ${SQLITE_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

if(WIN32)
target_link_libraries(${PROGNAME} Qt5::WinMain)
endif()

qt5_use_modules(${PROGNAME} Widgets OpenGL)

# install
install(TARGETS ${PROGNAME} DESTINATION bin)

if(UNIX AND NOT APPLE)
  find_package(KDE4)
  install(FILES konstruktor.desktop DESTINATION share/applications)
  install(FILES ldraw-mime.xml DESTINATION share/mime/packages)
  update_xdg_mimetypes(share/mime/packages)
  kde4_install_icons(${ICON_INSTALL_DIR})
endif(UNIX AND NOT APPLE)

