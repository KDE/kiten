INCLUDES = $(all_includes)

SUBDIRS = pics

kiten_SOURCES = 	main.cpp \
			kiten.cpp \
			optiondialog.cpp \
			learn.cpp \
			kloader.cpp \
			ksaver.cpp

kiten_LDFLAGS = $(all_libraries) $(KDE_RPATH) 
kiten_LDADD = libkiten.la $(LIB_KIO)

bin_PROGRAMS = kiten kitengen
lib_LTLIBRARIES = libkiten.la

libkiten_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined -version-info 1:0:0
libkiten_la_LIBADD = $(LIB_KDEUI) $(LIB_KDEPRINT)

libkiten_la_SOURCES = \
	asyndeta.cpp dict.cpp widgets.cpp kromajiedit.cpp rad.cpp deinf.cpp

libkitenincludedir = $(includedir)/libkiten
libkiteninclude_HEADERS = \
	asyndeta.h dict.h widgets.h kromajiedit.h rad.h deinf.h

kitengen_SOURCES = xjdxgen.c
kitengen_LDFLAGS = $(all_libraries) $(KDE_RPATH) 
kitengen_LDADD = $(LIB_QT)

METASOURCES = AUTO

CLEANFILES = dummy.cpp

rcdir = $(kde_datadir)/kiten
rc_DATA = kitenui.rc learnui.rc eeditui.rc

xdg_apps_DATA = kiten.desktop

kiten_DATA = edict kanjidic radkfile vconj romkana.cnv
kitendir = $(kde_datadir)/kiten

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kiten.pot
