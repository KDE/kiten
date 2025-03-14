/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#include <QApplication>
#include <QCommandLineParser>

#include "kiten.h"
#include "kiten_version.h"

void noMessageOutput(QtMsgType, const char *)
{
}

int main(int argc, char *argv[])
{
    //   qInstallMsgHandler(noMessageOutput);	//Disable Qt Errors from showing

    KLocalizedString::setApplicationDomain("kiten");

    QApplication app(argc, argv);

    KAboutData aboutData(QStringLiteral("kiten"),
                         i18n("Kiten"),
                         QStringLiteral(KITEN_VERSION_STRING),
                         i18n("Japanese Reference Tool"),
                         KAboutLicense::GPL_V2,
                         i18n("(c) 2001-2004, Jason Katz-Brown\n"
                              "(c) 2006-2007, Eric Kjeldergaard\n"
                              "(c) 2006-2008, Joseph Kerian\n"
                              "(c) 2011, Daniel E. Moctezuma"),
                         QString(),
                         QStringLiteral("https://apps.kde.org/kiten"));

    aboutData.addAuthor(i18n("Jason Katz-Brown"), i18n("Original author"), QStringLiteral("jason@katzbrown.com"));
    aboutData.addCredit(i18n("Jim Breen"),
                        i18n("Wrote xjdic, of which Kiten borrows code, and the xjdic index file generator.\n"
                             "Also is main author of edict and kanjidic, which Kiten essentially require."),
                        QStringLiteral("jwb@csse.monash.edu.au"));
    aboutData.addAuthor(i18n("Neil Stevens"), i18n("Code simplification, UI suggestions."), QStringLiteral("neil@qualityassistant.com"));
    aboutData.addCredit(i18n("David Vignoni"), i18n("svg icon"), QStringLiteral("david80v@tin.it"));
    aboutData.addCredit(i18n("Paul Temple"), i18n("Porting to KConfig XT, bug fixing"), QStringLiteral("paul.temple@gmx.net"));
    aboutData.addAuthor(i18n("Joseph Kerian"), i18n("KDE4 rewrite"), QStringLiteral("jkerian@gmail.com"));
    aboutData.addAuthor(i18n("Eric Kjeldergaard"), i18n("KDE4 rewrite"), QStringLiteral("kjelderg@gmail.com"));
    aboutData.addAuthor(i18n("Daniel E. Moctezuma"),
                        i18n("Deinflection system improvements, Dictionary updates for EDICT and KANJIDIC,\n"
                             "GUI Improvements, Kanji Browser, Bug fixes, Code polishing and simplification"),
                        QStringLiteral("democtezuma@gmail.com"));

    aboutData.setOrganizationDomain("kde.org");
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kiten")));

#ifdef Q_OS_WIN
    QApplication::setStyle(QStringLiteral("breeze"));
#endif

    KCrash::initialize();

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    auto kiten = new Kiten();
    kiten->show();

    return app.exec();
}
