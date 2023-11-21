/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "ui_searchdialog.h" //From the UI file

#include <QWidget>

class SearchDialog : public QDialog, public Ui::SearchDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);

Q_SIGNALS:
    void search(const QString &query);
};

#endif
