## Build kitengen, a small index generator used by the kiten app
add_executable(kitengen xjdxgen.c)
ecm_mark_nongui_executable(kitengen)
target_link_libraries(kitengen Qt::Core)

install(TARGETS kitengen ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
