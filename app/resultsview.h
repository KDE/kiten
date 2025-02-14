/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RESULTSVIEW_H
#define RESULTSVIEW_H

#include <QAction>
#include <QTextBrowser>

#include "entry.h"

class KActionCollection;
class KActionMenu;

class ResultsView : public QTextBrowser
{
    Q_OBJECT

public:
    explicit ResultsView(QWidget *parent = nullptr, const char *name = nullptr);

    void setLaterScrollValue(int scrollValue);

public Q_SLOTS:
    void append(const QString &text);
    void clear();
    void flush();
    void print(const QString &title);
    void setBasicMode(bool yes);
    void setContents(const QString &text);

Q_SIGNALS:
    void entrySpecifiedForExport(int index);
    void urlClicked(const QString &);

protected:
    QString generateCSS();

private Q_SLOTS:
    void doScroll();

private:
    QAction *_addToExportListAction = nullptr;
    bool _basicMode;
    KActionCollection *_popupActions = nullptr;
    KActionMenu *_popupMenu = nullptr;
    QString _printText;
    int _scrollValue;
};

#endif
