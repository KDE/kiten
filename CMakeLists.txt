project(kiten)

add_subdirectory( radselect )
add_subdirectory( bin )
add_subdirectory( lib )

############## kitengen (small EDICT format index generator) ############
set(kitengen_SRCS xjdxgen.c)
kde4_add_executable(kitengen ${kitengen_SRCS})
install( TARGETS kitengen DESTINATION ${BIN_INSTALL_DIR} )

############ install files #############
install( FILES data/edict data/kanjidic data/radkfile data/vconj data/romkana.cnv DESTINATION ${DATA_INSTALL_DIR}/kiten)
