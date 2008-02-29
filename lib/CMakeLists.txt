############# kitenlib #################
set(edict_SRCS
		dictEdict/dictFileEdict.cpp
		dictEdict/entryEdict.cpp
		dictEdict/dictFilePreferenceDialog.cpp)
set(kanjidic_SRCS
		dictKanjidic/dictFileKanjidic.cpp
		dictKanjidic/entryKanjidic.cpp)
set(dictionary_SRCS ${edict_SRCS} ${kanjidic_SRCS})


set(kiten_LIB_SRCS
		DictQuery.cpp
		DictionaryManager.cpp
		DictionaryPreferenceDialog.cpp
		Entry.cpp
		EntryList.cpp
		HistoryPtrList.cpp
		KitenEdit.cpp
		kromajiedit.cpp
		${dictionary_SRCS}
		)
kde4_add_library(kiten SHARED ${kiten_LIB_SRCS})
target_link_libraries(kiten ${KDE4_KDEUI_LIBS} )
set_target_properties(kiten PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install( TARGETS kiten  ${INSTALL_TARGETS_DEFAULT_ARGS})

############ install headers #############
install( FILES
		DictQuery.h
		DictionaryManager.h
		DictionaryPreferenceDialog.h
		Entry.h
		EntryList.h
		HistoryPtrList.h
		KitenEdit.h
		kromajiedit.h
		libkitenexport.h
	   DESTINATION ${INCLUDE_INSTALL_DIR}/libkiten
		)
