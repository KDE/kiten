include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

############## kiten binary #############
set(kiten_bin_SRCS 
	main.cpp
	kiten.cpp
	configdictionaryselector.cpp
	optiondialog.cpp
	widgets.cpp
	)
kde4_add_ui3_files(kiten_bin_SRCS 
	configsearching.ui
	configlearn.ui
	configfont.ui
	configdictselect.ui
	)
kde4_add_kcfg_files( kiten_bin_SRCS kitenconfig.kcfgc)
kde4_automoc(${kiten_bin_SRCS})
kde4_add_executable(kiten_bin ${kiten_bin_SRCS})
set_target_properties(kiten_bin PROPERTIES OUTPUT_NAME kiten)

target_link_libraries(kiten_bin ${KDE4_KDEUI_LIBS} kdeprint kiten)

install( TARGETS kiten_bin DESTINATION ${BIN_INSTALL_DIR} )

############ install files #############
install( FILES kiten.desktop DESTINATION ${XDG_APPS_DIR} )
install( FILES kiten.kcfg DESTINATION ${KCFG_INSTALL_DIR} )
install( FILES kitenui.rc DESTINATION ${DATA_INSTALL_DIR}/kiten )
