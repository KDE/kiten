############## kiten binary #############
set(kiten_bin_SRCS 
	main.cpp
	kiten.cpp
	resultsView.cpp
	configuredialog.cpp
	configdictionaryselector.cpp
	configsortingpage.cpp
	entryListView.cpp
	)
kde4_add_ui_files(kiten_bin_SRCS 
	configsearching.ui
	configlearn.ui
	configfont.ui
	configdictselect.ui
	configsorting.ui
	)
kde4_add_kcfg_files( kiten_bin_SRCS kitenconfig.kcfgc)

set(kiten_icons
    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/16x16/apps/kiten.png
    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/32x32/apps/kiten.png
    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/48x48/apps/kiten.png
    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/64x64/apps/kiten.png
    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/128x128/apps/kiten.png
)
kde4_add_win32_app_icon(kiten kiten_SRCS ${kiten_icons})
kde4_add_executable(kiten_bin ${kiten_bin_SRCS})
set_target_properties(kiten_bin PROPERTIES OUTPUT_NAME kiten)

target_link_libraries(kiten_bin ${KDE4_KHTML_LIBS} kiten)

install( TARGETS kiten_bin DESTINATION ${BIN_INSTALL_DIR} )

############ install files #############
install( FILES kiten.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES kiten.kcfg DESTINATION ${KCFG_INSTALL_DIR} )
install( FILES kitenui.rc DESTINATION ${DATA_INSTALL_DIR}/kiten )
