project(kiten)

#set(KITEN_BUILD_NEW true)

IF(KITEN_BUILD_NEW MATCHES true)
	
add_subdirectory( radselect )
add_subdirectory( bin )
add_subdirectory( lib )

############## kitengen (small EDICT format index generator) ############
set(kitengen_SRCS xjdxgen.c)
kde4_add_executable(kitengen ${kitengen_SRCS})
install( TARGETS kitengen DESTINATION ${BIN_INSTALL_DIR} )

############ install files #############
install( FILES data/edict data/kanjidic data/radkfile data/vconj data/romkana.cnv DESTINATION ${DATA_INSTALL_DIR}/kiten)

ELSE(KITEN_BUILD_NEW MATCHES true)
	
add_subdirectory( pics ) 

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

target_link_libraries(kiten_bin  ${KDE4_KDEUI_LIBS} kdeprint kiten)

install(TARGETS kiten_bin  DESTINATION ${BIN_INSTALL_DIR} )


########### next target ###############

set(kitengen_SRCS xjdxgen.c )

kde4_automoc(${kitengen_SRCS})

kde4_add_executable(kitengen ${kitengen_SRCS})

target_link_libraries(kitengen  ${KDE4_KDEUI_LIBS} )

install(TARGETS kitengen  DESTINATION ${BIN_INSTALL_DIR} )


########### next target ###############

set(kiten_LIB_SRCS asyndeta.cpp dict.cpp widgets.cpp kromajiedit.cpp rad.cpp deinf.cpp )


kde4_add_kcfg_files(kiten_LIB_SRCS kitenconfig.kcfgc)

kde4_automoc(${kiten_LIB_SRCS})

kde4_add_library(kiten SHARED ${kiten_LIB_SRCS})

target_link_libraries(kiten  ${KDE4_KDEUI_LIBS} )

set_target_properties(kiten PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS kiten  DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############

install( FILES kiten.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES kiten.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES edict kanjidic radkfile vconj romkana.cnv  DESTINATION  ${DATA_INSTALL_DIR}/kiten )
install( FILES asyndeta.h dict.h widgets.h kromajiedit.h rad.h deinf.h  DESTINATION  ${INCLUDE_INSTALL_DIR}/libkiten )
install( FILES kitenui.rc learnui.rc eeditui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kiten )




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
#include ../admin/Doxyfile.am
ENDIF(KITEN_BUILD_NEW MATCHES true)
