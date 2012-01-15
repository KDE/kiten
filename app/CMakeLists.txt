############## kiten binary #############
set(kiten_bin_SRCS
    configdictionaryselector.cpp
    configsortingpage.cpp
    configuredialog.cpp
    entrylistmodel.cpp
    entrylistview.cpp
    kiten.cpp
    main.cpp
    resultsview.cpp
    searchstringinput.cpp
    dictionaryupdatemanager.cpp
    wordtype.cpp
   )
kde4_add_ui_files(kiten_bin_SRCS
	configdictselect.ui
	configfont.ui
	configlearn.ui
	configsearching.ui
	configsorting.ui
	)
kde4_add_kcfg_files( kiten_bin_SRCS kitenconfig.kcfgc)

#kde4_add_app_icon(kiten_bin_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/kiten.png")
#kde4_add_app_icon(kiten_bin_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/pics/ox*-apps-kiten.png")
kde4_add_app_icon(kiten_bin_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/pics/hi*-apps-kiten.png")

kde4_add_executable(kiten_bin ${kiten_bin_SRCS})
set_target_properties(kiten_bin PROPERTIES OUTPUT_NAME kiten IMPORT_PREFIX bin_)

target_link_libraries(kiten_bin ${KDE4_KHTML_LIBS} kiten)

install( TARGETS kiten_bin ${INSTALL_TARGETS_DEFAULT_ARGS} )

############ install files #############
install( PROGRAMS kiten.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES kiten.kcfg DESTINATION ${KCFG_INSTALL_DIR} )
install( FILES kitenui.rc DESTINATION ${DATA_INSTALL_DIR}/kiten )
