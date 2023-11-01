############# kitenlib #################
set(KITEN_LIB_VERSION "5.0.0")
set(KITEN_LIB_SOVERSION "5")

add_library(kiten)
set_target_properties(kiten PROPERTIES
    VERSION ${KITEN_LIB_VERSION}
    SOVERSION ${KITEN_LIB_SOVERSION}
)

target_sources(kiten PRIVATE
    DictEdict/deinflection.cpp
    DictEdict/dictfileedict.cpp
    DictEdict/entryedict.cpp
    DictEdict/indexededictfile.cpp
    DictEdict/linearedictfile.cpp
    DictEdict/dictfilefieldselector.cpp

    DictKanjidic/dictfilekanjidic.cpp
    DictKanjidic/entrykanjidic.cpp

    dictionarymanager.cpp
    dictionarypreferencedialog.cpp
    dictquery.cpp
    entry.cpp
    entrylist.cpp
    historyptrlist.cpp
    kromajiedit.cpp
)

generate_export_header(kiten)

target_link_libraries(kiten
    PUBLIC
        KF6::Completion
        Qt::Widgets
        Qt::Core
    PRIVATE
		Qt6::Core5Compat
        KF6::ConfigCore
        KF6::ConfigGui
        KF6::CoreAddons
        KF6::I18n
        KF6::WidgetsAddons
)

if(WIN32)
    target_include_directories(kiten PRIVATE ${MMAN_INCLUDE_PATH})
    target_link_libraries(kiten PRIVATE ${MMAN_LIBRARY})
endif()

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
