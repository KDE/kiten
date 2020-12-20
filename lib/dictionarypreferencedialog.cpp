/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dictionarypreferencedialog.h"

#include <QString>

DictionaryPreferenceDialog::DictionaryPreferenceDialog( QWidget *parent, const QString &name )
: QWidget( parent )
, m_name( name )
{
}

DictionaryPreferenceDialog::~DictionaryPreferenceDialog()
{
}

QString DictionaryPreferenceDialog::name() const
{
  return m_name;
}


