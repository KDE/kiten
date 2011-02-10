#Name the project
project(kiten)

# Check for cmake, same as minimum for KDE 4.0.0
cmake_minimum_required( VERSION 2.4.5 )

# Check for KDE 4
find_package( KDE4 REQUIRED )
include( KDE4Defaults )
include_directories( ${KDE4_INCLUDES} )

## Make it easy for radselect and kiten to find libkiten headers
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

## Generate config.h
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
configure_file( config-kiten.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kiten.h )

## Build each subdir
add_subdirectory( doc )
add_subdirectory( radselect )
add_subdirectory( app )
add_subdirectory( data/pics )
add_subdirectory( lib )

## Build kitengen, a small index generator used by the kiten app
set(kitengen_SRCS xjdxgen.c)
kde4_add_executable(kitengen ${kitengen_SRCS})
target_link_libraries(kitengen ${QT_QTCORE_LIBRARY})
install( TARGETS kitengen ${INSTALL_TARGETS_DEFAULT_ARGS} )

## Install our datafiles
install( FILES data/edict data/kanjidic data/radkfile data/vconj data/romkana.cnv DESTINATION ${DATA_INSTALL_DIR}/kiten)
