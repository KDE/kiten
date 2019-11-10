cmake_minimum_required( VERSION 3.5 )

# KDE Application Version, managed by release script
set(KDE_APPLICATIONS_VERSION_MAJOR "19")
set(KDE_APPLICATIONS_VERSION_MINOR "11")
set(KDE_APPLICATIONS_VERSION_MICRO "80")
set(KDE_APPLICATIONS_VERSION "${KDE_APPLICATIONS_VERSION_MAJOR}.${KDE_APPLICATIONS_VERSION_MINOR}.${KDE_APPLICATIONS_VERSION_MICRO}")

#Name the project
project(kiten VERSION ${KDE_APPLICATIONS_VERSION})

#ECM setup
set(KF5_MIN_VERSION "5.46.0")
set(QT_REQUIRED_VERSION "5.11.0")
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_REQUIRED_VERSION} REQUIRED COMPONENTS Core Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Archive
    Completion 
    Config 
    ConfigWidgets 
    CoreAddons
    Crash
    DocTools 
    I18n 
    KHtml
    Notifications
    XmlGui)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(CheckIncludeFiles)
include(ECMGenerateHeaders)
include(ECMAddAppIcon)
include(ECMMarkNonGuiExecutable)
include(GenerateExportHeader)
include(ECMSetupVersion)

## Make it easy for radselect and kiten to find libkiten headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

## Find mman.h on Windows
if(WIN32)
    find_path(MMAN_INCLUDE_PATH NAMES sys/mman.h)
    if(NOT MMAN_INCLUDE_PATH)
        message(FATAL_ERROR "On Windows, you need to install mman-win32 headers.")
    endif()
    message(STATUS "Found mman-win32 include: ${MMAN_INCLUDE_PATH}")

    find_library(MMAN_LIBRARY NAMES mman)
    if(NOT MMAN_LIBRARY)
        message(FATAL_ERROR "On Windows, you need to install mman-win32 library.")
    endif()
    message(STATUS "Found mman-win32 library: ${MMAN_LIBRARY}")
endif(WIN32)

## Generate config.h
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
configure_file( config-kiten.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kiten.h )

add_definitions(-DTRANSLATION_DOMAIN=\"kiten\")
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
   add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x060000)
endif()


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
ecm_mark_nongui_executable(kitengen)
target_link_libraries(kitengen ${QT_QTCORE_LIBRARY})
install( TARGETS kitengen ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES org.kde.kiten.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

## Install our datafiles
install( FILES data/edict data/kanjidic data/radkfile data/vconj data/romkana.cnv DESTINATION ${DATA_INSTALL_DIR}/kiten )
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
