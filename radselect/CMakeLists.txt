############### radselect target ###############

set(radselect_bin_SRCS 
	main.cpp
	radselect.cpp
	radselectview.cpp
	radselectbuttongrid.cpp
	radicalbutton.cpp
	radicalfile.cpp
	)
kde4_add_ui_files(radselect_bin_SRCS radical_selector.ui radselectprefdialog.ui)
kde4_add_kcfg_files(radselect_bin_SRCS radselectconfig.kcfgc)

kde4_add_executable(radselect_bin ${radselect_bin_SRCS})
set_target_properties(radselect_bin PROPERTIES OUTPUT_NAME kitenradselect)
target_link_libraries(radselect_bin ${KDE4_KDEUI_LIBS} kiten)
install( TARGETS radselect_bin DESTINATION ${BIN_INSTALL_DIR} )

############## install files ####################
install( FILES radselect.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES radselectui.rc DESTINATION ${DATA_INSTALL_DIR}/kitenradselect )
