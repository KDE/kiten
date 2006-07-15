include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
#include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

############### radselect target ###############

set(radselect_bin_SRCS 
	radselectmain.cpp
	radselect.cpp
	radselectview.cpp
	radselectpref.cpp
	radselectbuttongrid.cpp
	)
kde4_add_ui_files(radselect_bin_SRCS radical_selector.ui)
#kde4_add_dcop_skels(radselect_bin_SRCS radselectiface.h)
kde4_automoc(${radselect_bin_SRCS})

kde4_add_executable(radselect_bin ${radselect_bin_SRCS})
set_target_properties(radselect_bin PROPERTIES OUTPUT_NAME kitenradselect)
target_link_libraries(radselect_bin ${KDE4_KDEUI_LIBS} kiten)
install( TARGETS radselect_bin DESTINATION ${BIN_INSTALL_DIR} )

############## install files ####################
install( FILES radselect.desktop DESTINATION ${XDG_APPS_DIR} )
install( FILES radselectui.rc DESTINATION ${DATA_INSTALL_DIR}/radselect )

#kde4_create_doxygen_docs( )
