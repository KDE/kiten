############# kitenlib #################
set(edict_SRCS
		DictEdict/dictfileedict.cpp
		DictEdict/entryedict.cpp
		DictEdict/indexededictfile.cpp
		DictEdict/linearedictfile.cpp
    DictEdict/dictfilefieldselector.cpp)
set(kanjidic_SRCS
		DictKanjidic/dictfilekanjidic.cpp
		DictKanjidic/entrykanjidic.cpp)
set(deinflect_SRCS
		DictDeinflect/dictfiledeinflect.cpp
    DictDeinflect/entrydeinflect.cpp)
set(dictionary_SRCS ${edict_SRCS} ${kanjidic_SRCS} ${deinflect_SRCS})

set(kiten_LIB_SRCS
		dictionarymanager.cpp
		dictionarypreferencedialog.cpp
		dictquery.cpp
		entry.cpp
		entrylist.cpp
		historyptrlist.cpp
		kromajiedit.cpp
		${dictionary_SRCS})

kde4_add_library(kiten SHARED ${kiten_LIB_SRCS})
target_link_libraries(kiten ${KDE4_KDEUI_LIBS})
set_target_properties(kiten PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
install( TARGETS kiten  ${INSTALL_TARGETS_DEFAULT_ARGS})

############ install headers #############
install( FILES
		dictionarymanager.h
		dictionarypreferencedialog.h
		dictquery.h
		entry.h
		entrylist.h
		historyptrlist.h
		kromajiedit.h
		libkitenexport.h
	  DESTINATION ${INCLUDE_INSTALL_DIR}/libkiten COMPONENT Devel
		)
