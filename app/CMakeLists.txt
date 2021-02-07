############## kiten binary #############

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KITEN VERSION_HEADER kiten_version.h)

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
ki18n_wrap_ui(kiten_bin_SRCS
        configdictselect.ui
        configfont.ui
        configlearn.ui
        configsearching.ui
        configsorting.ui
        )
kconfig_add_kcfg_files( kiten_bin_SRCS kitenconfig.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/pics/*-apps-kiten.png")
ecm_add_app_icon(kiten_bin_SRCS ICONS ${ICONS_SRCS})

add_executable(kiten_bin ${kiten_bin_SRCS})
set_target_properties(kiten_bin PROPERTIES OUTPUT_NAME kiten IMPORT_PREFIX bin_)

target_link_libraries(kiten_bin
    KF5::Archive
    KF5::ConfigWidgets
    KF5::Crash
    KF5::I18n
    KF5::KIOCore
    KF5::Notifications
    KF5::XmlGui
    kiten)

install( TARGETS kiten_bin ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

############ install files #############
install( PROGRAMS org.kde.kiten.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES kiten.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR} )
install( FILES kitenui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/kiten )
