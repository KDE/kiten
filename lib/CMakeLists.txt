#include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

############# kitenlib #################
set(kiten_LIB_SRCS
		kitenEdit.cpp
		kromajiedit.cpp
		historyPtrList.cpp
		deinf.cpp
		dictquery.cpp
		entry.cpp
		dictionary.cpp
		dictFileEDICT.cpp
		dictFileKanjidic.cpp
		entryEDICT.cpp
		entryKanjidic.cpp
		dictFilePreferenceDialog.cpp
		)
kde4_automoc(${kiten_LIB_SRCS})
kde4_add_library(kiten SHARED ${kiten_LIB_SRCS})
target_link_libraries(kiten ${KDE4_KDEUI_LIBS} )
set_target_properties(kiten PROPERTIES VERSION 1.0.1 SOVERSION 1 )
install( TARGETS kiten DESTINATION ${LIB_INSTALL_DIR} )

############ install files #############
install( FILES 
		dictquery.h 
		entry.h				dictionary.h
		entryEDICT.h 		dictFileEDICT.h 
		entryKanjidic.h 	dictFileKanjidic.h 
		historyPtrList.h 
		kromajiedit.h
		kitenEdit.h
		deinf.h 
		dictFilePreferenceDialog.h
	   DESTINATION ${INCLUDE_INSTALL_DIR}/libkiten	
		)

#kde4_create_doxygen_docs(  )
