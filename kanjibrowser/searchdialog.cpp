/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchdialog.h"

#include <QPushButton>

SearchDialog::SearchDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [this] {
        Q_EMIT search(lineEdit->text());
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
}

#include "moc_searchdialog.cpp"
