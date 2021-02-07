############### radselect target ###############

set(radselect_bin_SRCS
    buttongrid.cpp
    kanji.cpp
    main.cpp
    radical.cpp
    radicalbutton.cpp
    radicalfile.cpp
    radselect.cpp
    radselectview.cpp
   )
ki18n_wrap_ui(radselect_bin_SRCS radical_selector.ui radselectprefdialog.ui)
kconfig_add_kcfg_files(radselect_bin_SRCS radselectconfig.kcfgc)

add_executable(radselect_bin ${radselect_bin_SRCS})
set_target_properties(radselect_bin PROPERTIES OUTPUT_NAME kitenradselect)
target_link_libraries(radselect_bin 
    kiten 
    Qt5::Core 
    Qt5::Widgets
    KF5::ConfigWidgets 
    KF5::CoreAddons 
    KF5::I18n 
    KF5::XmlGui)
install( TARGETS radselect_bin ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

############## install files ####################
install( PROGRAMS org.kde.kitenradselect.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES radselectui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/kitenradselect )
