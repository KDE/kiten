#Name the project
project(kiten)

cmake_minimum_required( VERSION 2.8.12 )

#ECM setup
find_package(ECM 1.7 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(KF5 5.15 REQUIRED COMPONENTS
    Archive
    Completion 
    Config 
    ConfigWidgets 
    CoreAddons
    Crash
    DocTools 
    I18n 
    KHtml
    XmlGui)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMInstallIcons)
include( CheckIncludeFiles )
include(ECMGenerateHeaders)
include(ECMAddAppIcon)

## Make it easy for radselect and kiten to find libkiten headers
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

## Generate config.h
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
configure_file( config-kiten.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kiten.h )

add_definitions(-DTRANSLATION_DOMAIN=\"kiten\")


## Build each subdir
add_subdirectory( app )
add_subdirectory( data/font )
add_subdirectory( data/pics )
add_subdirectory( doc )
add_subdirectory( kanjibrowser )
add_subdirectory( lib )
add_subdirectory( radselect )

## Build kitengen, a small index generator used by the kiten app
set(kitengen_SRCS xjdxgen.c)
add_executable(kitengen ${kitengen_SRCS})
target_link_libraries(kitengen ${QT_QTCORE_LIBRARY})
install( TARGETS kitengen ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES org.kde.kiten.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

## Install our datafiles
install( FILES data/edict data/kanjidic data/radkfile data/vconj data/romkana.cnv DESTINATION ${DATA_INSTALL_DIR}/kiten )
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
