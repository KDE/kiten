kde4_header()

add_subdirectory( pics ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kiten_bin_SRCS ${kiten_LIB_SRCS} 
	main.cpp
	kiten.cpp 
	learn.cpp 
	kloader.cpp 
	ksaver.cpp 
	configdictionaries.cpp 
	optiondialog.cpp
	)
	
kde4_add_ui3_files(kiten_bin_SRCS configsearching.ui configlearn.ui configfont.ui configdictionariesbase.ui )


kde4_automoc(${kiten_bin_SRCS})


kde4_add_executable(kiten_bin ${kiten_bin_SRCS})
set_target_properties(kiten_bin PROPERTIES OUTPUT_NAME kiten)

target_link_libraries(kiten_bin  ${LIB_KDEUI} kdeprint kiten)

install_targets(/bin kiten_bin )


########### next target ###############

set(kitengen_SRCS xjdxgen.c )

kde4_automoc(${kitengen_SRCS})

kde4_add_executable(kitengen ${kitengen_SRCS})

target_link_libraries(kitengen  ${LIB_KDEUI} )

install_targets(/bin kitengen )


########### next target ###############

set(kiten_LIB_SRCS asyndeta.cpp dict.cpp widgets.cpp kromajiedit.cpp rad.cpp deinf.cpp )


kde4_add_kcfg_files(kiten_LIB_SRCS kitenconfig.kcfgc)

kde4_automoc(${kiten_LIB_SRCS})

kde4_add_library(kiten SHARED ${kiten_LIB_SRCS})

target_link_libraries(kiten  ${LIB_KDEUI} )

set_target_properties(kiten PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kiten )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES kiten.desktop )
install_files( ${KCFG_INSTALL_DIR} FILES kiten.kcfg )
install_files( ${DATA_INSTALL_DIR}/kiten FILES edict kanjidic radkfile vconj romkana.cnv )
install_files( /include/libkiten FILES asyndeta.h dict.h widgets.h kromajiedit.h rad.h deinf.h )
install_files( ${DATA_INSTALL_DIR}/kiten FILES kitenui.rc learnui.rc eeditui.rc )

kde4_create_doxygen_docs(  )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#
#SUBDIRS = pics
#
#kiten_SOURCES = main.cpp kiten.cpp learn.cpp kloader.cpp ksaver.cpp \
#	configsearching.ui configlearn.ui configfont.ui configdictionariesbase.ui \
#	configdictionaries.cpp optiondialog.cpp kitenconfig.kcfgc
#
#kiten_LDFLAGS = $(all_libraries) $(KDE_RPATH) 
#kiten_LDADD = libkiten.la $(LIB_KIO)
#
#bin_PROGRAMS = kiten kitengen
#lib_LTLIBRARIES = libkiten.la
#
#libkiten_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 1:0:0
#libkiten_la_LIBADD = $(LIB_KDEUI) $(LIB_KDEPRINT)
#
#libkiten_la_SOURCES = asyndeta.cpp dict.cpp widgets.cpp kromajiedit.cpp rad.cpp \
#	deinf.cpp kitenconfig.kcfgc
#
#libkitenincludedir = $(includedir)/libkiten
#libkiteninclude_HEADERS = \
#	asyndeta.h dict.h widgets.h kromajiedit.h rad.h deinf.h
#
#kitengen_SOURCES = xjdxgen.c
#kitengen_LDFLAGS = $(all_libraries) $(KDE_RPATH) 
#
#METASOURCES = AUTO
#
#rcdir = $(kde_datadir)/kiten
#rc_DATA = kitenui.rc learnui.rc eeditui.rc
#
#xdg_apps_DATA = kiten.desktop
#
#kiten_DATA = edict kanjidic radkfile vconj romkana.cnv
#kitendir = $(kde_datadir)/kiten
#
#kde_kcfg_DATA = kiten.kcfg
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kiten.pot
#
#
#
#
#
#include ../admin/Doxyfile.am
