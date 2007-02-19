############# kitenlib #################
set(edict_SRCS dictFileEdict.cpp entryEdict.cpp)
set(kanjidic_SRCS dictFileKanjidic.cpp entryKanjidic.cpp)
set(dictionary_SRCS ${edict_SRCS} ${kanjidic_SRCS})

set(kiten_LIB_SRCS
		kitenEdit.cpp
		kromajiedit.cpp
		historyPtrList.cpp
		dictQuery.cpp
		entry.cpp
		dictionary.cpp
		dictFilePreferenceDialog.cpp
		wordType.cpp
		${dictionary_SRCS}
		)
kde4_automoc(${kiten_LIB_SRCS})
kde4_add_library(kiten SHARED ${kiten_LIB_SRCS})
target_link_libraries(kiten ${KDE4_KDEUI_LIBS} )
set_target_properties(kiten PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install( TARGETS kiten DESTINATION ${LIB_INSTALL_DIR} )

############ install headers #############
install( FILES
		dictQuery.h
		entry.h
		dictionary.h
		historyPtrList.h
		kromajiedit.h
		kitenEdit.h
		wordType.h
		dictFilePreferenceDialog.h
	   DESTINATION ${INCLUDE_INSTALL_DIR}/libkiten
		)
