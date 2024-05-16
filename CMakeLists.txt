cmake_minimum_required( VERSION 3.16 )

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "24")
set(RELEASE_SERVICE_VERSION_MINOR "05")
set(RELEASE_SERVICE_VERSION_MICRO "0")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

#Name the project
project(kiten VERSION ${RELEASE_SERVICE_VERSION})

#ECM setup
set(KF_MIN_VERSION "5.240.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.240.0")
set(QT_REQUIRED_VERSION "6.5.0")
find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
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
include(KDEClangFormat)
include(KDEGitCommitHooks)

find_package(Qt6 ${QT_REQUIRED_VERSION} REQUIRED COMPONENTS Core DBus Widgets)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Archive
    Completion
    Config
    ConfigWidgets
    CoreAddons
    Crash
    DocTools
    I18n
    KIO
    XmlGui)

add_definitions(-DTRANSLATION_DOMAIN=\"kiten\")

## Build each subdir
add_subdirectory( app )
add_subdirectory( data/font )
add_subdirectory( data/pics )
add_subdirectory( doc )
add_subdirectory( kanjibrowser )
add_subdirectory( lib )
add_subdirectory( radselect )

install(FILES org.kde.kiten.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ki18n_install(po)
if (KF6DocTools_FOUND)
    kdoctools_install(po)
endif()

## Install our datafiles
install( FILES data/edict data/kanjidic data/radkfile data/vconj data/romkana.cnv DESTINATION ${KDE_INSTALL_DATADIR}/kiten )

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
