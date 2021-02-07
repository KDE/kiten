############### kanjibrowser target ###############

set(kanjibrowser_bin_SRCS
    kanjibrowser.cpp
    kanjibrowserview.cpp
    main.cpp
   )

ki18n_wrap_ui(kanjibrowser_bin_SRCS kanjibrowserview.ui preferences.ui)
kconfig_add_kcfg_files(kanjibrowser_bin_SRCS kanjibrowserconfig.kcfgc)

add_executable(kanjibrowser_bin ${kanjibrowser_bin_SRCS})
set_target_properties(kanjibrowser_bin PROPERTIES OUTPUT_NAME kitenkanjibrowser)
target_link_libraries(kanjibrowser_bin  
    kiten
    Qt5::Widgets
    KF5::Completion
    KF5::I18n
    KF5::XmlGui)
install( TARGETS kanjibrowser_bin ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

############## install files ####################
install( PROGRAMS org.kde.kitenkanjibrowser.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES kanjibrowserui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/kitenkanjibrowser )
