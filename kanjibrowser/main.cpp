/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QApplication>

#include "kanjibrowser.h"
#include <KAboutData>
#include <KLocalizedString>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kiten"), app.windowIcon()));
    KLocalizedString::setApplicationDomain("kiten");

    KAboutData about(QStringLiteral("kitenkanjibrowser"),
                     i18n("Kanji Browser"),
                     QStringLiteral("1.0"),
                     i18n("Kiten's Kanji Browser, a KDE Japanese Reference Tool"),
                     KAboutLicense::GPL_V2,
                     i18n("(C) 2011 Daniel E. Moctezuma"),
                     QString(),
                     QStringLiteral("https://edu.kde.org/kiten"),
                     QStringLiteral("democtezuma@gmail.com"));
    about.addAuthor(i18n("Daniel E. Moctezuma"), QString(), QStringLiteral("democtezuma@gmail.com"));
    about.setOrganizationDomain("kde.org");

    KAboutData::setApplicationData(about);

#ifdef Q_OS_WIN
    QApplication::setStyle(QStringLiteral("breeze"));
#endif

    if (app.isSessionRestored()) {
        kRestoreMainWindows<KanjiBrowser>();
    } else {
        auto kanjiBrowser = new KanjiBrowser();
        kanjiBrowser->show();
    }

    return app.exec();
}
