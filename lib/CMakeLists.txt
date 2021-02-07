############# kitenlib #################
set(KITEN_LIB_VERSION "5.0.0")
set(KITEN_LIB_SOVERSION "5")
set(edict_SRCS
		DictEdict/deinflection.cpp
		DictEdict/dictfileedict.cpp
		DictEdict/entryedict.cpp
		DictEdict/indexededictfile.cpp
		DictEdict/linearedictfile.cpp
    DictEdict/dictfilefieldselector.cpp)
set(kanjidic_SRCS
		DictKanjidic/dictfilekanjidic.cpp
		DictKanjidic/entrykanjidic.cpp)
set(dictionary_SRCS ${edict_SRCS} ${kanjidic_SRCS})

set(kiten_LIB_SRCS
		dictionarymanager.cpp
		dictionarypreferencedialog.cpp
		dictquery.cpp
		entry.cpp
		entrylist.cpp
		historyptrlist.cpp
		kromajiedit.cpp
		${dictionary_SRCS})

add_library(kiten SHARED ${kiten_LIB_SRCS})
generate_export_header(kiten)

target_link_libraries(kiten
		Qt5::Core
		Qt5::Widgets
		KF5::Completion
		KF5::ConfigCore
		KF5::ConfigGui
		KF5::CoreAddons
		KF5::I18n
		KF5::WidgetsAddons)

if(WIN32)
	target_include_directories(kiten PRIVATE ${MMAN_INCLUDE_PATH})
	target_link_libraries(kiten ${MMAN_LIBRARY})
endif()

set_target_properties(kiten PROPERTIES VERSION ${KITEN_LIB_VERSION} SOVERSION ${KITEN_LIB_SOVERSION})
install( TARGETS kiten  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

############ install headers #############
install( FILES
		dictionarymanager.h
		dictionarypreferencedialog.h
		dictquery.h
		entry.h
		entrylist.h
		historyptrlist.h
		kromajiedit.h
	  DESTINATION ${KDE_INSTALL_INCLUDEDIR}/libkiten COMPONENT Devel
		)
install(FILES
        DictEdict/dictfileedict.h
        DictEdict/entryedict.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}/libkiten/DictEdict COMPONENT Devel)
install(FILES
        DictKanjidic/dictfilekanjidic.h
        DictKanjidic/entrykanjidic.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}/libkiten/DictKanjidic COMPONENT Devel)
