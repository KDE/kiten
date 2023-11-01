############### kanjibrowser target ###############

add_executable(kanjibrowser_bin)
set_target_properties(kanjibrowser_bin PROPERTIES
    OUTPUT_NAME kitenkanjibrowser
)

target_sources(kanjibrowser_bin PRIVATE
    kanjibrowser.cpp
    kanjibrowser.h
    kanjibrowserview.cpp
    kanjibrowserview.h
    main.cpp

    kanjibrowser.qrc
)

ki18n_wrap_ui(kanjibrowser_bin
    kanjibrowserview.ui
    preferences.ui
)
kconfig_add_kcfg_files(kanjibrowser_bin kanjibrowserconfig.kcfgc)

target_link_libraries(kanjibrowser_bin  
    kiten
    Qt::Widgets
    KF6::Completion
    KF6::I18n
    KF6::XmlGui
)

install( TARGETS kanjibrowser_bin ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

install( PROGRAMS org.kde.kitenkanjibrowser.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
