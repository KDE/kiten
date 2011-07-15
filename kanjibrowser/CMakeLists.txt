############### kanjibrowser target ###############

set(kanjibrowser_bin_SRCS
    kanjibrowser.cpp
    kanjibrowserview.cpp
    main.cpp
   )

kde4_add_ui_files(kanjibrowser_bin_SRCS kanjibrowserview.ui)
kde4_add_kcfg_files(kanjibrowser_bin_SRCS kanjibrowserconfig.kcfgc)

kde4_add_app_icon(kanjibrowser_bin_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/pics/hi*-apps-kiten.png")

kde4_add_executable(kanjibrowser_bin ${kanjibrowser_bin_SRCS})
set_target_properties(kanjibrowser_bin PROPERTIES OUTPUT_NAME kitenkanjibrowser)
target_link_libraries(kanjibrowser_bin ${KDE4_KDEUI_LIBS} kiten)
install( TARGETS kanjibrowser_bin ${INSTALL_TARGETS_DEFAULT_ARGS} )

############## install files ####################
install( FILES kanjibrowserui.rc DESTINATION ${DATA_INSTALL_DIR}/kitenkanjibrowser )
