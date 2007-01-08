#Name the project
project(kiten)

## Make it easy for radselect and kiten to find libkiten headers
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib)

## Build each subdir
add_subdirectory( radselect )
add_subdirectory( app )
add_subdirectory( lib )

## Build kitengen, a small index generator used by the kiten app
set(kitengen_SRCS xjdxgen.c)
kde4_add_executable(kitengen ${kitengen_SRCS})
install( TARGETS kitengen DESTINATION ${BIN_INSTALL_DIR} )

## Install our datafiles
install( FILES data/edict data/kanjidic data/radkfile data/vconj data/romkana.cnv DESTINATION ${DATA_INSTALL_DIR}/kiten)
