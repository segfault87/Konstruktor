project(libldr)

set(libldr_SOURCES
  bfc.cpp
  color.cpp
  elements.cpp
  math.cpp
  metrics.cpp
  model.cpp
  part_library.cpp
  part_library_win32.cpp
  part_library_posix.cpp
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
  filter.h
  math.h
  metrics.h
  model.h
  part_library.h
  reader.h
  utils.h
  writer.h
)

add_definitions(-DMAKE_LIBLDR_LIB)

add_library(libldr SHARED ${libldr_SOURCES} ${libldr_HEADERS})
set_target_properties(libldr PROPERTIES OUTPUT_NAME ldraw)
set_target_properties(libldr PROPERTIES VERSION 0.5.0 SOVERSION 1)

# install
install(TARGETS libldr DESTINATION lib)
install(FILES ${libldr_HEADERS} DESTINATION include/libldr)

