INCLUDES = $(all_includes)

SUBDIRS = pics

kiten_SOURCES = 	main.cpp \
				dict.cpp \
				kiten.cpp \
				optiondialog.cpp \
				rad.cpp \
				deinf.cpp \
				kromajiedit.cpp \
				widgets.cpp \
				learn.cpp \
				kloader.cpp \
				ksaver.cpp

kiten_LDFLAGS = $(all_libraries) $(KDE_RPATH) 
kiten_LDADD = $(LIB_KDEPRINT) $(LIB_KIO)

bin_PROGRAMS = kiten kitengen

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
