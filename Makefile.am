INCLUDES = $(all_includes)

SUBDIRS = pics

lib_LTLIBRARIES = kiten.la

kiten_la_SOURCES = main.cpp \
			dict.cpp \
			kiten.cpp \
			optiondialog.cpp \
			widgets.cpp

kiten_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version

bin_PROGRAMS = kiten
kiten_LDADD = kiten.la $(KDE_RPATH) $(LIB_KDEUI)
kiten_LDFLAGS = $(all_libraries)
kiten_SOURCES = dummy.cpp

METASOURCES = AUTO

CLEANFILES = dummy.cpp

rcdir = $(kde_datadir)/kiten
rc_DATA = kitenui.rc

dummy.cpp:
	echo > dummy.cpp

applnk_DATA = kiten.desktop
applnkdir = $(kde_appsdir)/Utilities

datadir = $(kde_datadir)/kiten
