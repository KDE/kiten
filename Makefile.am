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

bin_PROGRAMS = kiten kitengen

kitengen_SOURCES = xjdxgen.c
kitengen_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_QT)

METASOURCES = AUTO

CLEANFILES = dummy.cpp

rcdir = $(kde_datadir)/kiten
rc_DATA = kitenui.rc learnui.rc eeditui.rc

applnk_DATA = kiten.desktop
applnkdir = $(kde_appsdir)/Edutainment/Languages

kiten_DATA = edict kanjidic radkfile vconj romkana.cnv
kitendir = $(kde_datadir)/kiten

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kiten.pot
