############### radselect target ###############

add_executable(radselect_bin)
set_target_properties(radselect_bin PROPERTIES
    OUTPUT_NAME kitenradselect
)

target_sources(radselect_bin PRIVATE
    buttongrid.cpp buttongrid.h
    kanji.cpp kanji.h
    main.cpp
    radical.cpp radical.h
    radicalbutton.cpp radicalbutton.h
    radicalfile.cpp radicalfile.h
    radselect.cpp radselect.h
    radselectview.cpp radselectview.h

    radselect.qrc
)
ki18n_wrap_ui(radselect_bin
    radical_selector.ui
    radselectprefdialog.ui
)
kconfig_add_kcfg_files(radselect_bin radselectconfig.kcfgc)

target_link_libraries(radselect_bin 
    kiten 
    Qt::Core
    Qt::DBus
    Qt::Widgets
    KF6::ConfigWidgets
    KF6::CoreAddons
    KF6::I18n
    KF6::XmlGui
)

install( TARGETS radselect_bin ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

install( PROGRAMS org.kde.kitenradselect.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
