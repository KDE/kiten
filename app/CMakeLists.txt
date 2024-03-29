############## kiten binary #############

add_executable(kiten_bin)
set_target_properties(kiten_bin PROPERTIES
    OUTPUT_NAME kiten
    IMPORT_PREFIX bin_
)

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KITEN VERSION_HEADER kiten_version.h)

target_sources(kiten_bin PRIVATE
    configdictionaryselector.cpp
    configdictionaryselector.h
    configsortingpage.cpp
    configsortingpage.h
    configuredialog.cpp
    configuredialog.h
    dictionaryupdatemanager.cpp
    dictionaryupdatemanager.h
    entrylistmodel.cpp
    entrylistmodel.h
    entrylistview.cpp
    entrylistview.h
    kiten.cpp
    kiten.h
    main.cpp
    resultsview.cpp
    resultsview.h
    searchstringinput.cpp
    searchstringinput.h

    kiten.qrc
)
ki18n_wrap_ui(kiten_bin
    configdictselect.ui
    configfont.ui
    configsearching.ui
    configsorting.ui
)
kconfig_add_kcfg_files( kiten_bin kitenconfig.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/pics/*-apps-kiten.png")
ecm_add_app_icon(kiten_bin ICONS ${ICONS_SRCS})

target_link_libraries(kiten_bin
    KF6::Archive
    KF6::ConfigWidgets
    KF6::Crash
    KF6::I18n
    KF6::KIOCore
    KF6::XmlGui
    kiten
)

install( TARGETS kiten_bin ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

install( PROGRAMS org.kde.kiten.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES kiten.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR} )
