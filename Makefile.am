INCLUDES = $(all_includes)

SUBDIRS = pics

lib_LTLIBRARIES = kiten.la

kiten_la_SOURCES = main.cpp \
			dict.cpp \
			kiten.cpp \
			optiondialog.cpp \
			widgets.cpp

kiten_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version

bin_PROGRAMS = kiten kitengen

kiten_LDADD = kiten.la $(KDE_RPATH) $(LIB_KDEUI) $(LIB_KFILE)
kiten_LDFLAGS = $(all_libraries)
kiten_SOURCES = dummy.cpp
kitengen_SOURCES = xjdxgen.c

METASOURCES = AUTO

CLEANFILES = dummy.cpp

rcdir = $(kde_datadir)/kiten
rc_DATA = kitenui.rc learnui.rc

dummy.cpp:
	echo > dummy.cpp

applnk_DATA = kiten.desktop
applnkdir = $(kde_appsdir)/Utilities

data_DATA = edict kanjidic
datadir = $(kde_datadir)/kiten

messages: $(XGETTEXT) *.cpp *.h -o $(podir)/Kiten.pot
