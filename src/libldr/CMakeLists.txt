project(libldr)

set(libldr_SOURCES
  bfc.cpp
  color.cpp
  elements.cpp
  math.cpp
  metrics.cpp
  model.cpp
  part_library.cpp
  reader.cpp
  utils.cpp
  writer.cpp
)

set(libldr_HEADERS
  bfc.h
  color.h 
  common.h
  elements.h
  exception.h
  extension.h
  math.h
  metrics.h
  model.h
  part_library.h
  reader.h
  utils.h
  writer.h
)

add_library(libldr SHARED ${libldr_SOURCES})
set_target_properties(libldr PROPERTIES OUTPUT_NAME ldraw)
set_target_properties(libldr PROPERTIES VERSION 0.5.0 SOVERSION 1)

# install
install(TARGETS libldr DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${libldr_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/libldr)

